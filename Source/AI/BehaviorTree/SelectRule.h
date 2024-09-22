#pragma once
#include "NodeBase.h"

class SelectRule
{
public:
	// 優先順位選択
	static NodeBase* SelectPriority(const std::vector<NodeBase*>& nodePool)
	{
		NodeBase* selectNode = nullptr;
		UINT priority = INT_MAX;

		// 一番優先順位が高いノードを探してselectNodeに格納
		for (NodeBase* node : nodePool)
		{
			if (node->priority >= priority) continue;

			priority = node->priority;
			selectNode = node;
		}

		return selectNode;
	}

	// ランダム選択
	NodeBase* SelectRandom(const std::vector<NodeBase*>& nodePool)
	{
		int selectNo = 0;
		// nodePoolのサイズで乱数を取得してselectNoに格納
		selectNo = Mathf::RandomRange(0, static_cast<int>(nodePool.size()));

		// nodePoolのselectNo番目の実態をリターン
		return nodePool.at(selectNo);
	}

	// シーケンス選択
	NodeBase* SelectSequence(const std::vector<NodeBase*>& nodePool, BehaviorData<Owner>* data)
	{
		int step = 0;

		// 指定されている中間ノードのシーケンスがどこまで実行されたか取得する
		step = data->GetSequenceStep(name);

		// 中間ノードに登録されているノード数以上の場合
		if (step >= children.size())
		{
			// ルールがBehaviorTree::SelectRule::Sequenceのときは次に実行できるノードがないため、nullptrをリターン
			if (selectRule == SelectRule::Sequence) {
				return nullptr;
			}
		}

		// 実行可能リストに登録されているデータの数だけループを行う
		for (auto itr = nodePool.begin(); itr != nodePool.end(); itr++)
		{
			// 子ノードが実行可能リストに含まれているか
			if ((*itr)->GetName() != children[step]->GetName()) continue;

			// 現在の実行ノードの保存、次に実行するステップの保存を行った後、
			// 現在のステップ番号のノードをリターン
			// ①スタックにはdata->PushSequenceNode関数を使用する。保存するデータは実行中の中間ノード。
			data->PushSequenceNode(this);

			// ②また、次に実行する中間ノードとステップ数を保存する
			//  保存にはdata->SetSequenceStep関数を使用。
			//  保存データは中間ノードの名前と次のステップ数(step + 1)
			data->SetSequenceStep(name, step + 1);

			// ③ステップ番号目の子ノードを実行ノードとしてリターンする
			return children.at(step);
		}
		// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
		return nullptr;
	}

	// on-off ランダム選択
	NodeBase* SelectRandom_OnOff(const std::vector<NodeBase*>& nodePool)
	{
		// nodePoolのサイズからvectorのコンテナを作る
		std::vector<int> rand;
		for (int i = 0; i < nodePool.size(); i++)
		{
			rand.emplace_back(i);
		}

		// nodePoolの数字をシャッフルする
		// 調べてそれっぽいので作りました
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		std::shuffle(rand.begin(), rand.end(), engine);

		// 前回の数字と比較
		int selectNo = *rand.begin();
		if (selectNo == OnOffNo)
		{
			// 同じだった場合一番前の数字を消去
			rand.erase(rand.begin());
			// 残った一番前の数字を selectNo に代入
			selectNo = *rand.begin();
		}

		OnOffNo = selectNo;
		return nodePool.at(selectNo);
	}

	// ルーレット選択
	NodeBase* SelectProbability(const std::vector<NodeBase*>& nodePool)
	{
		// nodePoolのサイズからvectorのコンテナを作る
		int total = 0;
		const int nodePoolCount = nodePool.size();

		std::vector<int> roulette;
		for (int i = 0; i < nodePoolCount; i++)
		{
			total += nodePool[i]->probability;
			roulette.emplace_back(nodePool[i]->probability);
		}

		// ルーレットの番号を取得
		int number = Mathf::RandomRange(0, total);

		total = 0;
		int answer = 0;
		for (int num : roulette)
		{
			total += num;

			if (total < number)
			{
				++answer;
				continue;
			}

			break;
		}

		return nodePool.at(answer);
	}
};