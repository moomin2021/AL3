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

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		// X、Y、Z軸周りの平行移動を設定
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

		// X、Y、Z方向のスケーリングを設定
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};

		// X、Y、Z軸周りの回転角を設定
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};

		// 度数法の角度で設定
		// worldTransform_.rotation_ = {XMConvertToRadians(45.0f), XMConvertToRadians(45.0f), 0.0f};

		// ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	// X、Y、Z軸周りの平行移動の数値をデバック表示
	std::string strDebug = std::string("translation(") +
		std::to_string(worldTransform_[0].translation_.x) + std::string(",") +
		std::to_string(worldTransform_[0].translation_.y) + std::string(",") +
		std::to_string(worldTransform_[0].translation_.z) + std::string(")");

	debugText_->Print(strDebug, 50, 50, 1.0f);

	// X、Y、Z軸周りの回転角をデバック表示
	strDebug = std::string("rotation:(") +
		std::to_string(worldTransform_[0].rotation_.x) + std::string(",") +
		std::to_string(worldTransform_[0].rotation_.y) + std::string(",") +
		std::to_string(worldTransform_[0].rotation_.z) + std::string(")");

	debugText_->Print(strDebug, 50, 70, 1.0f);

	// X、Y、Z軸周りの回転角をデバック表示
	strDebug = std::string("scale:(") +
		std::to_string(worldTransform_[0].scale_.x) + std::string(",") +
		std::to_string(worldTransform_[0].scale_.y) + std::string(",") +
		std::to_string(worldTransform_[0].scale_.z) + std::string(")");

	debugText_->Print(strDebug, 50, 90, 1.0f);
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
