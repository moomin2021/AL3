#include "GameScene.h"
#include "Easing.h"
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

	// 各オブジェクトの位置を設定
	worldTransform_[0].translation_ = {0.0f, +5.0f, 0.0f};
	worldTransform_[1].translation_ = {-5.0f, -5.0f, 0.0f};
	worldTransform_[2].translation_ = {+5.0f, -5.0f, 0.0f};

	// ワールドトランスフォームの初期化
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].Initialize();
	}

	// カメラ視点座標を設定
	viewProjection_.eye = {0, 0, -25};

	// カメラの注視点を設定
	//viewProjection_.target = worldTransform_[0].translation_;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	// SPACEキーを押したら注視対象を変更
	if (input_->TriggerKey(DIK_SPACE)) {

		// 注視対象番号に１を足す
		targetNum++;

		// 注視対象番号がオブジェクトの数と同じになったら注視対象番号を初期化
		if (_countof(worldTransform_) + 1 <= targetNum) targetNum = 1;


		// カメラの移動量を計算
		moveAmount.x = worldTransform_[targetNum - 1].translation_.x - viewProjection_.target.x;
		moveAmount.y = worldTransform_[targetNum - 1].translation_.y - viewProjection_.target.y;
		moveAmount.z = worldTransform_[targetNum - 1].translation_.z - viewProjection_.target.z;

		// カメラの移動時間をリセット
		cameraMoveTimer = 0;

		// 移動前の位置を保存
		savePos = viewProjection_.target;
	}

	if (cameraMoveTimer < cameraMoveTime) {

		// カメラ移動時間インクリメント
		cameraMoveTimer++;

		viewProjection_.target.x = savePos.x + (moveAmount.x * Easing::easeOutCubic(cameraMoveTimer / cameraMoveTime));
		viewProjection_.target.y = savePos.y + (moveAmount.y * Easing::easeOutCubic(cameraMoveTimer / cameraMoveTime));
		viewProjection_.target.z = savePos.z + (moveAmount.z * Easing::easeOutCubic(cameraMoveTimer / cameraMoveTime));
	}

	// 行列の再計算
	viewProjection_.UpdateMatrix();

	// テキスト表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f, %f, %f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf("target:(%f, %f, %f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf("up:(%f, %f, %f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	debugText_->SetPos(50, 110);
	debugText_->Printf("num:%d", targetNum);

	debugText_->SetPos(50, 130);
	debugText_->Printf("timer:%d", cameraMoveTimer);

	debugText_->SetPos(50, 150);
	debugText_->Printf("moveAmount:(%f, %f, %f)", moveAmount.x, moveAmount.y, moveAmount.z);

	debugText_->SetPos(50, 170);
	debugText_->Printf("result:%f", Easing::easeOutCubic(cameraMoveTimer / cameraMoveTime));
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
