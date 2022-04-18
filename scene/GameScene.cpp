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
	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	for (size_t i = 0; i < _countof(viewProjection_); i++) {
		viewProjection_[i].eye = {posDist(engine), posDist(engine), posDist(engine)};
		viewProjection_[i].Initialize();
	}
}

void GameScene::Update() {

	// SPACEを押すとカメラを切り替え
	if (input_->TriggerKey(DIK_SPACE)) {
		cameraNum++;
		if (cameraNum > 2) {
			cameraNum = 0;
		}
	}

	// 三つあるカメラのデバック情報を表示
	for (size_t i = 0; i < 3; i++) {
		debugText_->SetPos(50, 50 + i * 100);
		debugText_->Printf("Camera%d", i + 1);
		debugText_->SetPos(50, 70 + i * 100);
		debugText_->Printf(
		  "eye:(%f, %f, %f)", viewProjection_[i].eye.x, viewProjection_[i].eye.y,
		  viewProjection_[i].eye.z);
		debugText_->SetPos(50, 90 + i * 100);
		debugText_->Printf(
		  "target:(%f, %f, %f)", viewProjection_[i].target.x, viewProjection_[i].target.y,
		  viewProjection_[i].target.z);
		debugText_->SetPos(50, 110 + i * 100);
		debugText_->Printf(
		  "up:(%f, %f, %f)", viewProjection_[i].up.x, viewProjection_[i].up.y,
		  viewProjection_[i].up.z);
	}

	// 行列の再計算
	for (size_t i = 0; i < _countof(viewProjection_); i++) {
		viewProjection_[i].UpdateMatrix();
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

	model_->Draw(worldTransform_, viewProjection_[cameraNum], textureHandle_);

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
