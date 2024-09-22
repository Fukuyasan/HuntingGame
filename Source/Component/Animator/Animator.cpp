#include "Animator.h"

void Animator::Start()
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	// キャッシュの配列を resize
	nodeCaches.resize(object->GetModel()->GetResource()->GetNodes().size());
}

void Animator::UpdateAnimation(const float& elapsedTime)
{
	// アニメーション計算処理
	ComputeAnimation(elapsedTime);

	// ブレンド計算処理
	ComputeBlending(elapsedTime);

	// ルートモーション計算処理
	ComputeRootMotion(elapsedTime);
}

void Animator::PlayAnimation(int index, bool loop, float blendSeconds)
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	// アニメーション設定
	SetAnimation(index, loop);

	// ブレンド設定
	SetBlending(blendSeconds);

	if (object->GetModel()->rootMotionNodeIndex == -1) return;

	// ルートモーション設定
	SetRootMotion();
}

void Animator::SetAnimation(int index, bool loop)
{
	currentAnimationIndex   = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag       = loop;
	animationEndFlag        = false;
}

void Animator::SetBlending(float seconds)
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	blendFlag          = true;
	blendFirstFlag     = true;
	blendSeconds       = 0.0f;
	blendSecondsLength = seconds;

	// 現在の姿勢をキャッシュする
	const auto& nodeData = object->GetModel()->GetNodes();
	for (size_t i = 0; i < nodeData.size(); ++i)
	{
		const Model::Node& src = nodeData[i];
		NodeCache& dst		   = nodeCaches[i];

		dst.scale     = src.scale;
		dst.rotate    = src.rotate;
		dst.translate = src.translate;
	}
}

void Animator::SetRootMotion()
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	rootMotionTranslation      = { 0,0,0 };
	//cacheRootMotionTranslation = { 0,0,0 };

	auto& rootMotionNode = object->GetModel()->GetNodes()[object->GetModel()->rootMotionNodeIndex];
	cacheRootMotionTranslation = rootMotionNode.translate;
}

bool Animator::IsPlayAnimation() const
{
	std::shared_ptr<GameObject> object = gameObject.lock();
	size_t animSize = object->GetModel()->GetResource()->GetAnimations().size();

	if (currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= animSize) return false;

	return true;
}

void Animator::UpdateRootMotion(Transform& transform)
{
	std::shared_ptr<GameObject> object = gameObject.lock();

	if (!rootMotionFlag) return;
	if (object->GetModel()->rootMotionNodeIndex < 0) return;

	Model::Node& rootMotionNode = object->GetModel()->GetNodes()[object->GetModel()->rootMotionNodeIndex];

	// 変換したグローバル移動差分値のY成分を０にする。
	rootMotionTranslation.y = 0;

	// 今回のルートモーションノードのグローバル位置を算出する。
	DirectX::XMMATRIX ParentGlobalTransform = DirectX::XMLoadFloat4x4(&rootMotionNode.parent->globalTransform);
	DirectX::XMVECTOR RootMotionTranslate = DirectX::XMLoadFloat3(&rootMotionNode.translate);
	DirectX::XMVECTOR RootGlobalTransform = DirectX::XMVector3TransformNormal(RootMotionTranslate, ParentGlobalTransform);

	// 算出したグローバル位置のXZ成分を０にする。
	DirectX::XMFLOAT3 globalTransform;
	DirectX::XMStoreFloat3(&globalTransform, RootGlobalTransform);

	globalTransform.x = 0;
	globalTransform.z = 0;

	RootGlobalTransform = DirectX::XMLoadFloat3(&globalTransform);

	// グローバル位置をローカル位置に変換する。
	DirectX::XMMATRIX ParentLocalTransform = DirectX::XMMatrixInverse(nullptr, ParentGlobalTransform);
	DirectX::XMVECTOR LocalTransform = DirectX::XMVector3TransformCoord(RootGlobalTransform, ParentLocalTransform);

	// 変換したローカル位置をルートモーションノードに設定する。
	rootMotionNode.translate.x = DirectX::XMVectorGetX(LocalTransform);
	rootMotionNode.translate.y = DirectX::XMVectorGetY(LocalTransform);
	rootMotionNode.translate.z = DirectX::XMVectorGetX(LocalTransform);

	// グローバル移動差分値をキャラクターのワールド行列に乗算する。
	DirectX::XMVECTOR RootMotionTranslation = DirectX::XMLoadFloat3(&rootMotionTranslation);
	DirectX::XMVECTOR GlobalTransform = DirectX::XMVector3TransformNormal(RootMotionTranslation, ParentGlobalTransform);

	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&transform.m_transform);
	DirectX::XMVECTOR WorldTranslation = DirectX::XMVector3TransformNormal(GlobalTransform, WorldTransform);

	transform.m_position.x += DirectX::XMVectorGetX(WorldTranslation);
	transform.m_position.y += DirectX::XMVectorGetY(WorldTranslation);
	transform.m_position.z += DirectX::XMVectorGetZ(WorldTranslation);

	rootMotionTranslation = { 0,0,0 };
}

void Animator::ComputeAnimation(const float& elapsedTime)
{
	// アニメーション再生中でないなら処理しない
	if (!IsPlayAnimation()) return;

	std::shared_ptr<GameObject> object = gameObject.lock();

	// アニメーション情報を取得
	const auto& animations                    = object->GetModel()->GetResource()->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);
	const auto& keyframes                     = animation.keyframes;

	// アニメーションの長さを取得
	animationLength = animation.secondsLength;

	// 最終フレーム処理
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}

	// 時間経過
	currentAnimationSeconds += elapsedTime * animationSpeed;

	// 再生時間が終端時間を超えたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		// 再生時間を巻き戻す
		if (animationLoopFlag)
		{
			currentAnimationSeconds -= animation.secondsLength;

			// ルートモーションを戻しておく
			if (object->GetModel()->rootMotionNodeIndex != -1)
			{
				cacheRootMotionTranslation = rootMotionTranslation;
			}
		}
		// 再生終了時間にする
		else
		{
			rootMotionFlag = false;
			animationEndFlag = true;
			currentAnimationSeconds = animation.secondsLength;
		}

		return;
	}

	// アニメーション情報からキーフレーム情報を取得
	size_t keyCount = keyframes.size();
	for (size_t keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		size_t nextKeyIndex = keyIndex + 1;

		// 現在の時間がどのキーフレームの間にいるか判定する
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(nextKeyIndex);

		// キーフレーム内でないなら continue
		if (currentAnimationSeconds < keyframe0.seconds ||
			currentAnimationSeconds > keyframe1.seconds) continue;

		// 再生時間とキーフレームの時間から補完率算出
		float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

		size_t nodeCount = object->GetModel()->GetNodes().size();
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			// 2つのキーフレーム間の補完計算
			const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
			const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

			auto& node = object->GetModel()->GetNodes()[nodeIndex];

			// 前のキーフレームと次のキーフレームの姿勢を補完
			DirectX::XMVECTOR S = DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&key0.scale),
				DirectX::XMLoadFloat3(&key1.scale),
				rate);

			DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(
				DirectX::XMLoadFloat4(&key0.rotate),
				DirectX::XMLoadFloat4(&key1.rotate),
				rate);

			DirectX::XMVECTOR T = DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&key0.translate),
				DirectX::XMLoadFloat3(&key1.translate),
				rate);

			object->GetModel()->SetNodeTransforms(nodeIndex, S, R, T);

			// 初回の位置を取得
			if (nodeIndex == object->GetModel()->rootMotionNodeIndex && 0 >= keyframe0.seconds && 0 <= keyframe1.seconds)
			{
				DirectX::XMStoreFloat3(&beginRootMotionTranslation, T);
			}
		}
		break;
	}

	rootMotionFlag = true;
}

void Animator::ComputeBlending(const float& elapsedTime)
{
	if (!blendFlag) return;

	std::shared_ptr<GameObject> object = gameObject.lock();

	// 時間更新
	blendSeconds += elapsedTime;

	if (blendSeconds >= blendSecondsLength)
	{
		blendSeconds = blendSecondsLength;
		blendFlag = false;
		return;
	}

	// ブレンド率の計算
	float rate = blendSeconds / blendSecondsLength;

	// ブレンド計算
	size_t count = object->GetModel()->GetNodes().size();
	for (size_t nodeIndex = 0; nodeIndex < count; ++nodeIndex)
	{
		const NodeCache& cache = nodeCaches.at(nodeIndex);
		auto& node = object->GetModel()->GetNodes()[nodeIndex];

		DirectX::XMVECTOR S = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&cache.scale),
			DirectX::XMLoadFloat3(&node.scale),
			rate);

		DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(
			DirectX::XMLoadFloat4(&cache.rotate),
			DirectX::XMLoadFloat4(&node.rotate),
			rate);

		DirectX::XMVECTOR T = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&cache.translate),
			DirectX::XMLoadFloat3(&node.translate),
			rate);

		object->GetModel()->SetNodeTransforms(nodeIndex, S, R, T);
	}

	rootMotionFlag = true;
}

void Animator::ComputeRootMotion(const float& elapsedTime)
{
	if (!rootMotionFlag) return;

	std::shared_ptr<GameObject> object = gameObject.lock();
	if (object->GetModel()->rootMotionNodeIndex < 0) return;

	auto& rootMotionNode = object->GetModel()->GetNodes()[object->GetModel()->rootMotionNodeIndex];

	// 前回のボーンの位置から今回の位置のボーンの差分値を取得
	rootMotionTranslation.x = rootMotionNode.translate.x - cacheRootMotionTranslation.x;
	rootMotionTranslation.y = rootMotionNode.translate.y - cacheRootMotionTranslation.y;
	rootMotionTranslation.z = rootMotionNode.translate.z - cacheRootMotionTranslation.z;

	cacheRootMotionTranslation = rootMotionNode.translate;
}
