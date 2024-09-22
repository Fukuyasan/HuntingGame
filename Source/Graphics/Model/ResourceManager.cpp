#include "ResourceManager.h"
#include "System/Logger.h"
#include "Graphics/Graphics.h"

// 初期化
void ResourceManager::Initialize()
{		
	// イベント作成
	event = CreateEvent(NULL, false, false, NULL);
	
	// スレッド開始
	thread = std::make_unique<std::thread>([this] { LoadingThread(); });
}

// 終了化
void ResourceManager::Finalize()
{
	// スレッド終了要求
	exitRequested = true;

	// スレッドが終了するまで待つ
	if (thread != nullptr)
	{
		WakeUpLoadingThread();
		thread->join();
	}

	// イベント終了
	if(event != NULL)
	{
		CloseHandle(event);
		event = NULL;
	}

	// 管理しているリソースクリア
	penddingModels.clear();
	loadedModels.clear();
}

// 読み込み中のリソースが読み込み完了するまで待つ
void ResourceManager::WaitOnPending()
{
	size_t numPending = 0;
	do
	{
		std::lock_guard<std::mutex> lock(mutex);

		numPending = penddingModels.size();

		if (numPending)
		{
			Sleep(16);
		}
	} while (numPending);
}

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	// モデルを検索
	ModelMap::iterator it = loadedModels.find(filename);
	if (it != loadedModels.end())
	{
		// リンク（寿命）が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みのモデルリソースを返す
			return it->second.lock();
		}
	}

	// 新規モデルリソース作成＆読み込み
	ID3D11Device* device = Graphics::Instance().GetDevice();
	auto model = std::make_shared<ModelResource>();
	model->Load(device, filename);

	// マップに登録
	loadedModels[filename] = model;

	return model;
}

// モデルリソース読み込み(非同期)
std::shared_ptr<ModelResource> ResourceManager::LoadModelResourceAsync(const char* filename)
{
	std::lock_guard<std::mutex> lock(mutex);

	std::shared_ptr<ModelResource> resource;

	// 読み込み済みリソースがあるかチェック
	{
		ModelMap::iterator it = loadedModels.find(filename);
		if (it != loadedModels.end())
		{
			// リンクが切れていないかチェック
			if (!it->second.expired())
			{
				resource = it->second.lock();
			}
		}
	}

	// 読み込みリソースがあるかチェック
	if (resource == nullptr)
	{
		ModelMap::iterator it = penddingModels.find(filename);
		if (it != penddingModels.end())
		{
			resource = it->second.lock();
		}
	}

	// 新規リソースを作成し、読み込み中リストに追加
	if (resource == nullptr)
	{
		resource = std::make_shared<ModelResource>();
		penddingModels[filename] = resource;
		// スレッドを起こす
		WakeUpLoadingThread();
	}

	return resource;
}

// ローディングスレッド
void ResourceManager::LoadingThread()
{
	while (!exitRequested)
	{
		// スレッドが起きるまで待つ
		{
			WaitForSingleObject(event, INFINITE);
		}
		if (exitRequested) break;

		// モデルリソース
		LoadModel();
	}
}

// モデルを読み込む
void ResourceManager::LoadModel()
{
	// 読み込み処理
	ID3D11Device* device = Graphics::Instance().GetDevice();

	std::string filename;
	bool pending = true;
	while (pending)
	{
		// モデルリソース
		{
			std::shared_ptr<ModelResource> model;
			// マップから読み込み情報取得
			{
				std::lock_guard<std::mutex> lock(mutex);

				// 先頭の読み込み情報を取得
				ModelMap::iterator it = penddingModels.begin();

				// 読み込むものがなければ終了
				if (it == penddingModels.end())
				{
					pending = false;
					continue;
				}

				// これから読み込むファイル名情報取得
				filename = it->first;

				// これから読み込むリソースを取得
				model = it->second.lock();
			}
			// 読み込み
			model->Load(device, filename.c_str());
			LOG("Loaded %s\n",  filename.c_str());

			// 読み込み済みリソースの登録と読み込み中リソースの解除
			{
				std::lock_guard<std::mutex> lock(mutex);

				loadedModels[filename] = model;
				penddingModels.erase(filename);
			}
		}
	}
}

// スレッドを起こす
void ResourceManager::WakeUpLoadingThread()
{
	if (event != NULL)
	{
		SetEvent(event);
	}
}
