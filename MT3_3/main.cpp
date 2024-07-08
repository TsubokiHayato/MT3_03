#include <Novice.h>
#include"Vector3.h"
#include<Matrix.h>

#include<ImGuiManager.h>

const char kWindowTitle[] = "学籍番号";


// Linear interpolation between two vectors v1 and v2
Vector3 Leap(const Vector3& v1, const Vector3& v2, float t) {
	return v1 + (v2 - v1) * t;
}

// Draw a Bezier curve defined by three control points
void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	// Bezier curve calculation (assuming quadratic Bezier)
	const float step = 0.01f; // Step size for interpolation
	Vector3 lastPoint = controlPoint0;

	for (float t = step; t <= 1.0f; t += step) {
		Vector3 currentPoint = Leap(Leap(controlPoint0, controlPoint1, t),
			Leap(controlPoint1, controlPoint2, t),
			t);

		// Transform to viewport space
		Vector3 transformedPoint = Vector3(
			viewProjectionMatrix.m[0][0] * currentPoint.x + viewProjectionMatrix.m[0][1] * currentPoint.y + viewProjectionMatrix.m[0][2] * currentPoint.z + viewProjectionMatrix.m[0][3],
			viewProjectionMatrix.m[1][0] * currentPoint.x + viewProjectionMatrix.m[1][1] * currentPoint.y + viewProjectionMatrix.m[1][2] * currentPoint.z + viewProjectionMatrix.m[1][3],
			viewProjectionMatrix.m[2][0] * currentPoint.x + viewProjectionMatrix.m[2][1] * currentPoint.y + viewProjectionMatrix.m[2][2] * currentPoint.z + viewProjectionMatrix.m[2][3]
		);

		transformedPoint.x /= transformedPoint.z;
		transformedPoint.y /= transformedPoint.z;

		transformedPoint.x = (transformedPoint.x + 1) * 0.5f * viewportMatrix.m[0][0] + viewportMatrix.m[3][0];
		transformedPoint.y = (transformedPoint.y + 1) * 0.5f * viewportMatrix.m[1][1] + viewportMatrix.m[3][1];

		// Code for drawing the point at transformedPoint in viewport space with specified color
		// (Implementation depends on your rendering API, e.g., OpenGL, DirectX)
		// drawPoint(transformedPoint, color);

		lastPoint = currentPoint;
	}
}


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	const float kGridHalfWidth = 2.0f;//Gridの半分の幅
	const uint32_t kSubdivision = 10;//分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);//ひとつ分の長さ

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + xIndex * kGridEvery;

		Vector3 startX = { x, 0.0f, -kGridHalfWidth };
		Vector3 endX = { x, 0.0f, kGridHalfWidth };

		startX = Transform(startX, Multiply(viewProjectionMatrix, viewportMatrix));
		endX = Transform(endX, Multiply(viewProjectionMatrix, viewportMatrix));




		for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
			float z = -kGridHalfWidth + zIndex * kGridEvery;

			Vector3 startZ = { -kGridHalfWidth, 0.0f, z };
			Vector3 endZ = { kGridHalfWidth, 0.0f, z };

			startZ = Transform(startZ, Multiply(viewProjectionMatrix, viewportMatrix));
			endZ = Transform(endZ, Multiply(viewProjectionMatrix, viewportMatrix));

			Novice::DrawLine((int)startX.x, (int)startX.y, (int)endX.x, (int)endX.y, 0xaaaaaaff);
			Novice::DrawLine((int)startZ.x, (int)startZ.y, (int)endZ.x, (int)endZ.y, 0xaaaaaaff);


		}
	}

}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 rotate = {};
	Vector3 translate{};

	Vector3 cameraPosition{ 0.0f,-.0f,-6.49f };
	Vector3 cameraScale{ 1.0f, 1.0f, 1.0f };
	Vector3 cameraRotate{ 2.6f,0.0f,0.0f };


	Vector3 controlPoints[3] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f}
	};

	uint32_t color = 0xFF0000;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		 // 更新
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraPosition);
		Matrix4x4 projectionMatrix = MakePerspectiveMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(cameraMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		ImGui::DragFloat3("cameraPos", &cameraPosition.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("cameraScale", &cameraScale.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f, -10.0f, 10.0f);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		// Draw Bezier curve
		DrawBezier(controlPoints[0], controlPoints[1], controlPoints[2], worldViewProjectionMatrix, viewportMatrix, color);


		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
