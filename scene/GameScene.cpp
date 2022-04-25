#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {

	// モデルの削除
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの作成
	model_ = Model::Create();

	// 乱数シード生成器
	std::random_device seed_gen;

	// メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());

	// 乱数範囲（回転角用）
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);

	// 乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	// ワールドトランスフォームの初期化
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].Initialize();
	}

	// 親の設定
	worldTransform_[0].Initialize();

	// 子の設定

	// (1)
	worldTransform_[1].translation_ = {-2.0f, 0.0f, -2.0f};
	worldTransform_[1].parent_ = &worldTransform_[0];
	worldTransform_[1].Initialize();

	// (2)
	worldTransform_[2].translation_ = {0.0f, 0.0f, -2.0f};
	worldTransform_[2].parent_ = &worldTransform_[0];
	worldTransform_[2].Initialize();

	// (3)
	worldTransform_[3].translation_ = {2.0f, 0.0f, -2.0f};
	worldTransform_[3].parent_ = &worldTransform_[0];
	worldTransform_[3].Initialize();

	// カメラ視点座標を設定
	viewProjection_.eye = {0, 10, -15};

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	
	// 前方ベクトル
	XMFLOAT3 forwardVec = {0, 0, 1};

	// 計算結果
	XMFLOAT3 resultVec = {0, 0, 0};

	// 移動ベクトル
	XMFLOAT3 move = {0, 0, 0};

	resultVec.x = cosf(worldTransform_[0].rotation_.y) * forwardVec.x +
	              sinf(worldTransform_[0].rotation_.y) * forwardVec.z;
	resultVec.z = -sinf(worldTransform_[0].rotation_.y) * forwardVec.x +
	              cosf(worldTransform_[0].rotation_.y) * forwardVec.z;

	// Dキー入力で右回転
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_[0].rotation_.y += 0.1f;
	}
	
	// Aキー入力で左回転
	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_[0].rotation_.y -= 0.1f;
	}

	// Wキー入力で前方に移動
	if (input_->PushKey(DIK_UP)) {
		move = {resultVec.x, 0, resultVec.z};
	}

	// Sキー入力で前方に移動
	if (input_->PushKey(DIK_DOWN)) {
		move = {-resultVec.x, 0, -resultVec.z};
	}

	worldTransform_[0].translation_.x += move.x * 0.2f;
	worldTransform_[0].translation_.y += move.y * 0.2f;
	worldTransform_[0].translation_.z += move.z * 0.2f;

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	// Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	// スプライト描画後処理
	// Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dモデル描写

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
