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

	//for (size_t i = 0; i < _countof(worldTransform_); i++) {

	//	// X、Y、Z軸周りの平行移動を設定
	//	worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

	//	// X、Y、Z方向のスケーリングを設定
	//	worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};

	//	// X、Y、Z軸周りの回転角を設定
	//	worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};

	//	// 度数法の角度で設定
	//	// worldTransform_.rotation_ = {XMConvertToRadians(45.0f), XMConvertToRadians(45.0f), 0.0f};

	//	// ワールドトランスフォームの初期化
	//	worldTransform_[i].Initialize();
	//}

	// キャラクターの大元を初期化
	worldTransform_[PartId::Root].Initialize();

	// 脊椎の初期化と設定
	worldTransform_[PartId::Spine].translation_ = {0, 4.5f, 0};// --------------> 脊椎のローカル座標を設定
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];// -> 脊椎の親を大元に設定
	worldTransform_[PartId::Spine].Initialize();// -----------------------------> 脊椎を初期化

	// -上半身の初期化と設定- //

	// 胸
	worldTransform_[PartId::Chest].translation_ = {0, 0, 0};// -------------------> 胸のローカル座標を設定
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];// --> 胸の親を脊椎に設定
	worldTransform_[PartId::Chest].Initialize();// -------------------------------> 胸を初期化

	// 頭
	worldTransform_[PartId::Head].translation_ = {0, 3, 0};// ------------------> 頭のローカル座標を設定
	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];// -> 頭の親を胸に設定
	worldTransform_[PartId::Head].Initialize();// ------------------------------> 頭の初期化

	// 左腕
	worldTransform_[PartId::ArmL].translation_ = {-3, 0, 0};// -----------------> 左腕のローカル座標を設定
	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];// -> 左腕の親を胸に設定
	worldTransform_[PartId::ArmL].Initialize();// ------------------------------> 左腕の初期化

	// 右腕
	worldTransform_[PartId::ArmR].translation_ = {3, 0, 0};// -----------------> 右腕のローカル座標を設定
	worldTransform_[PartId::ArmR].parent_ =&worldTransform_[PartId::Chest];// -> 右腕の親を胸に設定
	worldTransform_[PartId::ArmR].Initialize();// -----------------------------> 右腕の初期化

	// -下半身の初期化- //

	// 尻
	worldTransform_[PartId::Hip].translation_ = {0, -3, 0};// -----------------> 尻のローカル座標を設定
	worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];// -> 尻の親を脊椎に設定
	worldTransform_[PartId::Hip].Initialize();// ------------------------------> 尻の初期化

	// 左足
	worldTransform_[PartId::LegL].translation_ = {-3, -3, 0};// --------------> 左足のローカル座標を設定
	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];// -> 左足の親を尻に設定
	worldTransform_[PartId::LegL].Initialize();// ----------------------------> 左足の初期化

	// 右足
	worldTransform_[PartId::LegR].translation_ = {3, -3, 0};// ---------------> 右足のローカル座標を設定
	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];// -> 右足の親を尻に設定
	worldTransform_[PartId::LegR].Initialize();// ----------------------------> 右足の初期化

	// カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	// アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;

	// ニアクリップ距離を設定
	//viewProjection_.nearZ = 52.0f;

	// ファークリップ距離を設定
	//viewProjection_.farZ = 53.0f;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	////////////////////
	////	-視点移動処理-	//
	////////////////////

	//// 視点の移動ベクトル
	//XMFLOAT3 move = {0, 0, 0};

	//// 視点の移動速さ
	//const float kEyeSpeed = 0.2f;

	//// 推した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_W)) {
	//	move = {0, 0, kEyeSpeed};
	//} else if (input_->PushKey(DIK_S)) {
	//
	//move = {0, 0, -kEyeSpeed};
	//}

	//// 視点移動（ベクトルの加算）
	//viewProjection_.eye.x += move.x;
	//viewProjection_.eye.y += move.y;
	//viewProjection_.eye.z += move.z;

	////////////////////////
	////	-注視点移動処理-	//
	////////////////////////

	//// 注視点の移動ベクトル
	//move = {0, 0, 0};

	//// 注視点び移動速さ
	//const float kTargetSpeed = 0.2f;

	//// 押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_LEFT)) {
	//	move = {-kTargetSpeed, 0, 0};
	//} else if (input_->PushKey(DIK_RIGHT)) {
	//	move = {kTargetSpeed, 0, 0};
	//}

	//// 注視点移動（ベクトルの加算）
	//viewProjection_.target.x += move.x;
	//viewProjection_.target.y += move.y;
	//viewProjection_.target.z += move.z;

	////////////////////////
	////	-上方向回転処理-	//
	////////////////////////

	//// 上方向の回転速さ[ラジアン/frame]
	//const float kUpRotSpeed = 0.05f;

	//// 押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_SPACE)) {
	//	viewAngle += kUpRotSpeed;

	//	// 2nを超えたら0に戻す
	//	viewAngle = fmodf(viewAngle, XM_2PI);
	//}

	//// 上方向ベクトルを計算（半径1の円周上の座標）
	//viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};

	//////////////////
	//	-Fov変更処理-	//
	//////////////////

	// Wキーで視野角が広がる
	//if (input_->PushKey(DIK_W)) {
	//	viewProjection_.fovAngleY += 0.01f;
	//	viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);

	// Sキーで視野角が狭まる
	//} else if (input_->PushKey(DIK_S)) {
	//	viewProjection_.fovAngleY -= 0.01f;
	//	viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
	//}

	//////////////////////////
	//	-クリップ距離変更処理-	//
	//////////////////////////

	// 上下キーでニアクリップ距離を増減
	//if (input_->PushKey(DIK_UP)) {
	//	viewProjection_.nearZ += 0.1f;
	//} else if (input_->PushKey(DIK_DOWN)) {
	//	viewProjection_.nearZ -= 0.1f;
	//}

	//////////////////////////
	//	-キャラクター移動処理-	//
	//////////////////////////

	// キャラクターの移動ベクトル
	XMFLOAT3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		move = {-kCharacterSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kCharacterSpeed, 0, 0};
	}

	//////////////////////
	//	-上半身回転処理-	//
	//////////////////////

	// 上半身の回転の速さ[ラジアン/frame]
	const float kChestRotSpeed = 0.05f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_U)) {
		worldTransform_[PartId::Chest].rotation_.y -= kChestRotSpeed;
	} else if (input_->PushKey(DIK_I)) {
		worldTransform_[PartId::Chest].rotation_.y += kChestRotSpeed;
	}

	//////////////////////
	//	-下半身回転処理-	//
	//////////////////////

	// 下半身の回転の速さ[ラジアン/frame]
	const float kHipRotSpeed = 0.05f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_J)) {
		worldTransform_[PartId::Hip].rotation_.y -= kHipRotSpeed;
	} else if (input_->PushKey(DIK_K)) {
		worldTransform_[PartId::Hip].rotation_.y += kHipRotSpeed;
	}

	// 注視点移動（ベクトルの加算）
	worldTransform_[PartId::Root].translation_.x += move.x;
	worldTransform_[PartId::Root].translation_.y += move.y;
	worldTransform_[PartId::Root].translation_.z += move.z;

	// オブジェクト更新
	worldTransform_[PartId::Root].UpdateMatrix();
	worldTransform_[PartId::Spine].UpdateMatrix();
	worldTransform_[PartId::Chest].UpdateMatrix();
	worldTransform_[PartId::Head].UpdateMatrix();
	worldTransform_[PartId::ArmL].UpdateMatrix();
	worldTransform_[PartId::ArmR].UpdateMatrix();
	worldTransform_[PartId::Hip].UpdateMatrix();
	worldTransform_[PartId::LegL].UpdateMatrix();
	worldTransform_[PartId::LegR].UpdateMatrix();

	//////////////////////
	// -デバック用表示-	//
	//////////////////////

	// 視点座標を表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f, %f, %f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	// 注視点座標を表示
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f, %f, %f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	// 上方向座標を表示
	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f, %f, %f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	// 垂直方向視野角を表示
	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));

	// ニアクリップを表示
	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);

	// 親の座標を表示
	debugText_->SetPos(50, 150);
	debugText_->Printf(
	  "Root(%f, %f, %f)", worldTransform_[PartId::Root].translation_.x, worldTransform_[PartId::Root].translation_.y,
	  worldTransform_[PartId::Root].translation_.z);

	// アスペクト比を表示
	debugText_->SetPos(50, 170);
	debugText_->Printf("aspectRatio:%f", viewProjection_.aspectRatio);

	// 行列の再計算
	viewProjection_.UpdateMatrix();

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
	//for (size_t i = 0; i < _countof(worldTransform_); i++) {
	//	model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	//}

	//model_->Draw(worldTransform_[PartId::Root], viewProjection_, textureHandle_);
	//model_->Draw(worldTransform_[PartId::Spine], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Head], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmR], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Hip], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegR], viewProjection_, textureHandle_);

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
