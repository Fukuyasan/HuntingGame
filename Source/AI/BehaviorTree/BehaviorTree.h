#pragma once
#include <string>
#include "JudgmentBase.h"
#include "ActionBase.h"

#define USE_STRING 0

template<class Owner> class NodeBase;
template<class Owner> class BehaviorData;

// 選択ルール
enum class SelectRule
{
	Non,				// 無い末端ノード用
	Priority,			// 優先順位
	Sequence,			// シーケンス
	Random,				// ランダム
	Random_OnOff,       // ランダム　オンオフ法　
	Probability,        // 確率　ルーレット法
};

template<class Owner>class BehaviorTree
{
public:
	BehaviorTree(Owner* enemy) :root(nullptr) , owner(enemy){}

	~BehaviorTree() {
		NodeAllClear(root);
	}

	// 実行ノードを推論する
	NodeBase<Owner>* ActiveNodeInference(BehaviorData<Owner>* data)
	{
		// データをリセットして開始
		data->Init();
		return root->Inference(data);
	}

	// シーケンスノードから推論開始
	NodeBase<Owner>* SequenceBack(NodeBase<Owner>* sequenceNode, BehaviorData<Owner>* data)
	{
		return sequenceNode->Inference(data);
	}

	// ノード追加
	void AddNode(int parentName, int entryName, int priority, SelectRule selectRule, int probability = 0, JudgmentBase<Owner>* judgment = nullptr, ActionBase<Owner>* action = nullptr)
	{
		// Root(一番上のルート)じゃない場合
		if (parentName != rootNon)
		{
			// 親ノードを探す
			NodeBase<Owner>* parentNode = root->SearchNode(parentName);

			if (parentNode != nullptr)
			{
				NodeBase<Owner>* sibling = parentNode->GetLastChild();
				NodeBase<Owner>* addNode = new NodeBase<Owner>(entryName, parentNode, sibling, priority, selectRule, probability, judgment, action, parentNode->GetHirerchyNo() + 1);

				parentNode->AddChild(addNode);
			}
		}
		// Root(一番上のルート)の場合
		else
		{
			if (root == nullptr)
			{
				root = new NodeBase<Owner>(entryName, nullptr, nullptr, priority, selectRule, probability, judgment, action, 1);
			}
		}
	}

	// 割り込みノード追加
	void AddInterruptionNode(int entryName, JudgmentBase<Owner>* judgment, ActionBase<Owner>* action = nullptr)
	{
		NodeBase<Owner>* interruptionNode = new NodeBase<Owner>(entryName, nullptr, nullptr, 0, SelectRule::Non, 0, judgment, action, 2);
		root->AddInterruption(interruptionNode);
	}

	// 実行開始処理
	bool Start(NodeBase<Owner>* actionNode)
	{
		return actionNode->Start();
	}

	// 実行
	NodeBase<Owner>* Run(NodeBase<Owner>* actionNode, BehaviorData<Owner>* data, const float& elapsedTime)
	{
		// ノード実行
		ActionState state;
		
		// ノード実行
		state = actionNode->Run(elapsedTime);

		// 割り込み
		int interruptionCount = root->GetInterruptionSize();
		for (int i = 0; i < interruptionCount; ++i)
		{
			NodeBase<Owner>* inte = root->GetInterruption(i);

			if (!inte->judgment->Judgment()) continue;
			if (inte->name == actionNode->name) continue;
			
			state = actionNode->Stop();
		}
				
		// 正常終了
		if (state == ActionState::Complete)
		{
			// シーケンスの途中かを判断
			NodeBase<Owner>* sequenceNode = data->PopSequenceNode();

			// 途中ならそこから始める
			if (sequenceNode)
			{
				return SequenceBack(sequenceNode, data);
			}			

			// ないなら終了
			return nullptr;
		}
		// 失敗は終了
		if (state == ActionState::Failed)
		{
			return nullptr;
		}

		// 現状維持
		return actionNode;
	}
private:
	// ノード全削除
	void NodeAllClear(NodeBase<Owner>* _node)
	{
		for (NodeBase<Owner>* node : _node->children)
		{
			NodeAllClear(node);
		}

		for (NodeBase<Owner>* node : _node->interruption)
		{
			NodeAllClear(node);
		}

		delete _node;
	}
private:
	// ルートノード
	NodeBase<Owner>* root = nullptr;
	Owner* owner          = nullptr;

	const int rootNon = -1;
};