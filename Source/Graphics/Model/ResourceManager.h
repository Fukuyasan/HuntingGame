#pragma once

#include <memory>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ModelResource.h"

class ResourceManager
{
private:
	ResourceManager()  = default;
	~ResourceManager() = default;
public:
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&)		= delete;
	ResourceManager operator= (const ResourceManager&) = delete;
	ResourceManager operator= (ResourceManager&&)	   = delete;

	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 読み込み中のリソースが読み込み完了するまで待つ
	void WaitOnPending();

	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
	std::shared_ptr<ModelResource> LoadModelResourceAsync(const char* filename);
private:
	// ローディングスレッド
	void LoadingThread();

	// モデル
	void LoadModel();

	// スレッドを起こす
	void WakeUpLoadingThread();
private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;

	// モデルを読み込む用のマップ
	ModelMap penddingModels;
	// モデルをロードする用のマップ
	ModelMap loadedModels;

	std::unique_ptr<std::thread> thread;
	std::mutex					 mutex;
	HANDLE						 event = NULL;

	// volatile : Releaseだと最適化のため、boolが消えてしまうらしいのでそれを防ぐ
	volatile bool exitRequested = false;
};
