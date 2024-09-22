#pragma once

#include <vector>
#include <stack>
#include <map>
#include "NodeBase.h"

// Behavior保存データ
template<class Owner>class BehaviorData
{
public:
	// コンストラクタ
	BehaviorData() { Init(); }
	// シーケンスノードのプッシュ
	void PushSequenceNode(NodeBase<Owner>* node) { sequenceStack.push(node); }
	
	// シーケンスノードのポップ
	NodeBase<Owner>* PopSequenceNode()
	{
		// 空ならNULL
		if (sequenceStack.empty()) return nullptr;
		
		NodeBase<Owner>* node = sequenceStack.top();
		if (node)
		{
			// 取り出したデータを削除
			sequenceStack.pop();
		}
		return node;
	}

	// シーケンスステップのゲッター
	int GetSequenceStep(int name)
	{
		if (runSequenceStepMap.count(name) == 0)
		{
			runSequenceStepMap.insert(std::make_pair(name, 0));
		}

		return runSequenceStepMap[name];
	}

	// シーケンスステップのセッター
	void SetSequenceStep(int name, int step) { runSequenceStepMap[name] = step; }

	// 初期化
	void Init()
	{
		runSequenceStepMap.clear();
		while (sequenceStack.size() > 0)
		{
			sequenceStack.pop();
		}
	}
private:
	std::stack<NodeBase<Owner>*> sequenceStack;  // 実行する中間ノードをスタック
	std::map<int, int> runSequenceStepMap;		 // 実行中の中間ノードのステップを記録
};
