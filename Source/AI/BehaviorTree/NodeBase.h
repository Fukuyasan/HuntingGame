#pragma once

#include <vector>
#include <string>
#include <random>

#include "BehaviorTree.h"
#include "System/Math/Mathf.h"

// メモリリーク調査用
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// ノード
template<class Owner>class NodeBase
{
public:
	// コンストラクタ
	NodeBase(int name, NodeBase* parent, NodeBase* sibling, int priority, SelectRule selectRule,
		int Probability, JudgmentBase<Owner>* judgment, ActionBase<Owner>* action, int hierarchyNo) :
		name(name), parent(parent), sibling(sibling), priority(priority), 
		selectRule(selectRule), probability(Probability), judgment(judgment), action(action), hierarchyNo(hierarchyNo),
		children(NULL) {}

	// デストラクタ
	~NodeBase() 
	{
		delete judgment;
		delete action;
	}

public:
#pragma region ゲッター セッター
	// 名前ゲッター
	int GetName() { return name; }
	// 親ノードゲッター
	NodeBase* GetParent() { return parent; }
	// 子ノードゲッター
	NodeBase* GetChild(int index) {	return (index < children.size()) ? children.at(index) : nullptr; }
	// 子ノードゲッター(末尾)
	NodeBase* GetLastChild() { return (!children.empty()) ? children.at(children.size() - 1) : nullptr; }
	// 子ノードゲッター(先頭)
	NodeBase* GetTopChild()	{ return  (!children.empty()) ? children.at(0) : nullptr; }
	// 兄弟ノードゲッター
	NodeBase* GetSibling() { return sibling; }
	// 階層番号ゲッター
	int GetHirerchyNo() { return hierarchyNo; }
	// 優先順位ゲッター
	int GetPriority() { return priority; }
	// 親ノードセッター
	void SetParent(NodeBase* parent) { this->parent = parent; }
	// 子ノード追加
	void AddChild(NodeBase* child) { children.emplace_back(child); }
	// 兄弟ノードセッター
	void SetSibling(NodeBase* sibling) {this->sibling = sibling;}
	// 行動データを持っているか
	bool HasAction() { return action; }
	int ReSetStep() { return action->step = 0; }
	int GetStep() { return action->step; }
	// 実行可否判定
	// judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。
	bool Judgment()	{ return (judgment) ? judgment->Judgment() : true; }

	// 割り込みノード
	void AddInterruption(NodeBase* child) { interruption.emplace_back(child); }
	NodeBase* GetInterruption(int index) { return interruption[index]; }
	int GetInterruptionSize() { return static_cast<int>(interruption.size()); }
#pragma endregion

public:
	// 優先順位選択
	NodeBase* SelectPriority(const std::vector<NodeBase*>& nodePool)
	{
		NodeBase* selectNode  = nullptr;
		UINT priority         = INT_MAX;

		// 一番優先順位が高いノードを探してselectNodeに格納
		for (NodeBase* node : nodePool)
		{
			if (node->priority >= priority) continue;

			priority   = node->priority;
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
		const size_t nodePoolCount = nodePool.size();

		std::vector<int> roulette;
		for (size_t i = 0; i < nodePoolCount; i++)
		{
			total += nodePool[i]->probability;
			roulette.emplace_back(nodePool[i]->probability);
		}

		// ルーレットの番号を取得
		int number = Mathf::RandomRange(0, total);
		total	   = 0;
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

public:
	// ノード検索
	NodeBase* SearchNode(int searchName)
	{
		// 名前が一致
		if (name == searchName)
		{
			return this;
		}
		
		// 子ノードで検索
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			auto result = (*itr)->SearchNode(searchName);
			if (result)
			{
				return result;
			}
		}

		return nullptr;
	}

	// ノード推論
	NodeBase* Inference(BehaviorData<Owner>* data)
	{
		std::vector<NodeBase*> nodePool;
		NodeBase* result = nullptr;

		// childrenの数だけループを行う。
		for (NodeBase* child : children)
		{
			// 判定クラスがなければ無条件に追加
			if (!child->judgment)
			{
				nodePool.emplace_back(child);
				continue;
			}

			// 条件があり、その条件を満たしていれば追加
			if (child->judgment->Judgment()) nodePool.emplace_back(child);
		}

		// 選択ルールでノード決め
		switch (selectRule)
		{
		case SelectRule::Priority:
			result = SelectPriority(nodePool);
			break;
		case SelectRule::Random:
			result = SelectRandom(nodePool);
			break;
		case SelectRule::Sequence:
			result = SelectSequence(nodePool, data);
			break;
		case SelectRule::Random_OnOff:
			result = SelectRandom_OnOff(nodePool);
			break;
		case SelectRule::Probability:
			result = SelectProbability(nodePool);
			break;
		}

		if (result)
		{
			// resultにactionノードがなかった場合、
			// 再びノード推論
			if (!result->HasAction())
			{
				result = result->Inference(data);
			}
			else
			{
				result->Start();
			}
		}

		return result;
	}

	// 実行
	bool Start()
	{
		// actionがあるか判断。あればメンバ関数Start()を実行
		if (!action) return false;

		return action->Start();
	}

	// 実行
	ActionState Run(const float& elapsedTime)
	{
		// actionがあるか判断。あればメンバ関数Run()実行した結果をリターン。
		if (!action) return ActionState::Failed;
		
		return action->Run(elapsedTime);
	}

	// 実行中の行動を停止
	ActionState Stop()
	{
		return ActionState::Failed;
	}

	std::vector<NodeBase*> children;		    // 子ノード
	std::vector<NodeBase*> interruption;		// 中断ノード
public:
	int							name;			    // 名前
	SelectRule					selectRule;			// 選択ルール
	JudgmentBase<Owner>*		judgment;			// 判定クラス
	ActionBase<Owner>*			action;				// 実行クラス
	unsigned int				priority;		    // 優先順位
	NodeBase*					parent;			    // 親ノード
	NodeBase*					sibling;		    // 兄弟ノード
	int							hierarchyNo;	    // 階層番号
	int                         OnOffNo = INT_MAX;  // on-off用の切り替え変数
	int                         probability;  // ルーレットによる確率（大きければその分確率は高くなる）
};