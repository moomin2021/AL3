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
		angle[i] = (360.0f / 10) * i;
		worldTransform_[i].Initialize();
	}

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	for (size_t i = 0; i < _countof(angle); i++) {
		angle[i] += 2.0f;
		if (angle[i] >= 360.0f) angle[i] = 0.0f;

		worldTransform_[i].translation_.x = cos(angle[i] * 3.14f / 180.0f) * 10.0f;
		worldTransform_[i].translation_.y = sin(angle[i] * 3.14f / 180.0f) * 10.0f;
	}

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].UpdateMatrix();
	}

	if (input_->PushKey(DIK_W)) {
		viewProjection_.eye.z += 0.2f;
	}

	if (input_->PushKey(DIK_S)) {
		viewProjection_.eye.z -= 0.2f;
	}

	// 行列の再計算
	viewProjection_.UpdateMatrix();

	

	// テキスト表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "translation:(%f, %f, %f)", worldTransform_[0].translation_.x,
	  worldTransform_[0].translation_.y, worldTransform_[0].translation_.z);
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
