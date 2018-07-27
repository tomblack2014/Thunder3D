#include "camera.h"
#include <gl\GL.h>

// {9307E154-61FE-477E-A285-BB9D10CB7F97}
const GUID ICamera::IID_ICamera =
{ 0x9307e154, 0x61fe, 0x477e, {0xa2, 0x85, 0xbb, 0x9d, 0x10, 0xcb, 0x7f, 0x97} };

Camera::Camera(_In_ const Vec4f& eye) :
	refcnt(0),
	pos(eye)
{
}

ULONG Camera::AddRef() {
	refcnt++;
	return refcnt;
}

ULONG Camera::Release() {
	--refcnt;
	if (refcnt == 0) {
		delete this;
		return 0;
	}
	else {
		return refcnt;
	}
}

HRESULT Camera::QueryInterface(_In_ const IID&id, _Outptr_ void**ppv) {
	if (id == IID_IUnknown) {
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (id == IID_IObject) {
		*ppv = static_cast<IObject*>(this);
	}
	else if (id == IID_ICamera) {
		*ppv = static_cast<ICamera*>(this);
	}
	else {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

Vec4f Camera::GetPos() {
	return pos;
}

void Camera::Frame(_In_ float /*interval*/) {
}

void Camera::Render() {
}

Vec4f Camera::GetEye2At()
{
	return Vec4f();
}

Vec4f Camera::GetUp()
{
	return Vec4f();
}

void* Camera::operator[](_In_opt_ const std::string& name)
{
	if (name == "pos") {
		return &pos;
	}
	else {
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////

const float FreeCamera::SLERP_TIME = 1.0f;

FreeCamera::FreeCamera(_In_ const Vec4f& eye, _In_ const Vec4f& at, _In_ const Vec4f& up) :
	Camera(eye),
	at(at),
	up(up),
	lookMat(Mat4f::lookat(pos, this->at, this->up)),
	motionMat(Mat4f::eye()),
	motion0(Mat4f::eye()),
	motion1(Mat4f::eye()),
	inSlerp(false),
	tim(0.0f)
{
}

void FreeCamera::LoadMatrix() {
	Mat4f realMat = motionMat * lookMat;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(realMat.m);
}

void FreeCamera::MoveOnScreen(_In_ const Vec4f& trans) {
	motionMat = Mat4f::translate(trans) * motionMat;
}

void FreeCamera::RotateLR(_In_ float ang) {
	Vec4f yx(0.0f, 1.f, 0.0f);
	motionMat = Mat4f::rotate(yx, ang) * motionMat;
}

void FreeCamera::RotateUD(_In_ float ang) {
	Vec4f xx(1.0f, 0.f, 0.0f);
	motionMat = Mat4f::rotate(xx, ang) * motionMat;
}

Vec4f FreeCamera::GetEye2At()
{
	Mat4f realMat = motionMat * lookMat;
	return realMat.GetEye2At();
}

Vec4f FreeCamera::GetUp()
{
	Mat4f realMat = motionMat * lookMat;
	return realMat.GetUp();
}

void FreeCamera::MakeBillBoard() {
	Mat4f m = (motionMat * lookMat).transpose();
	m.m[3] = m.m[7] = m.m[11] = m.m[12] = m.m[13] = m.m[14] = 0.0f;
	glMultMatrixf(m.m);
}

void FreeCamera::RecordPos() {
	motion0 = motion1;
	motion1 = motionMat;
}

void FreeCamera::StartSlerp() {
	inSlerp = true;
	tim = 0;
	motionMat = motion0;
}

void FreeCamera::Frame(_In_ float interval) {
	if (!inSlerp)
		return;
	tim += interval / SLERP_TIME;
	if (tim >= 1.0f) {
		tim = 1.0f;
		inSlerp = false;
	}

	Vec4f q0 = Vec4f::QuatFromMat(motion0);
	Vec4f p0 = motion0.FindEyePos();

	Vec4f q1 = Vec4f::QuatFromMat(motion1);
	Vec4f p1 = motion1.FindEyePos();

	Vec4f qt = Vec4f::slerp(q0, q1, tim);
	Vec4f pt = Vec4f::lerp(p0, p1, tim);

	motionMat = qt.QuatToMat() * Mat4f::translate(-pt);
}

FreeCamera* FreeCamera::create(_In_ const Vec4f& eye, _In_ const Vec4f& at, _In_ const Vec4f& up) {
	FreeCamera* ret = new FreeCamera(eye, at, up);
	ret->AddRef();
	return ret;
};

//////////////////////////////////////////////////////////////////////////

TraceCamera::TraceCamera(_In_ IObject* obj) :
	Camera(obj->GetPos()),
	obj(obj),
	lookMat(Mat4f::eye())
{
	obj->AddRef();
}

TraceCamera::~TraceCamera()
{
	SafeRelease(&obj);
}

void TraceCamera::Frame(_In_ float /*interval*/)
{
	Vec4f at = obj->GetPos();
	Vec4f eye = at - Vec4f(0.0f, 0.0f, 10.0f);
	Vec4f up(0.0f, 1.0f, 0.0f);
	lookMat = Mat4f::lookat(eye, at, up);
}

void TraceCamera::LoadMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lookMat.m);
}

void TraceCamera::MakeBillBoard() {
	Mat4f m = lookMat.transpose();
	m.m[3] = m.m[7] = m.m[11] = m.m[12] = m.m[13] = m.m[14] = 0.0f;
	glMultMatrixf(m.m);
}

TraceCamera* TraceCamera::create(_In_ IObject* obj)
{
	TraceCamera* ret = new TraceCamera(obj);
	ret->AddRef();
	return ret;
}

//////////////////////////////////////////////////////////////////////////

CraftCamera::CraftCamera(_In_ Craft* craft) :
	Camera(craft->GetPos()),
	craft(craft),
	MAX_DIS(16.f),
	MIN_DIS(8.f),
	m_curDis(12.f),
	m_mode(0),
	CHANGDE_MODE_CD(1.f),
	m_changeModeCD(-1.f)
{
	craft->AddRef();
}

CraftCamera::~CraftCamera() {
	SafeRelease(&craft);
}

void CraftCamera::Frame(_In_ float interval) {
	Vec4f at = craft->GetPos();
	Vec4f forw = craft->GetSpeed();
	forw.normal();
	Vec4f up = craft->GetUp();
	Vec4f eye = at + forw * m_curDis * (m_mode == 0 ? -1 : 1);

	eye = eye + up * 1.2f;
	at = at + up * 1.0f;
	pos = eye;

	lookMat = Mat4f::lookat(eye, at, up);

	m_changeModeCD -= interval;
}

void CraftCamera::LoadMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lookMat.m);
}

void CraftCamera::MakeBillBoard() {
	Mat4f m = lookMat.transpose();
	m.m[3] = m.m[7] = m.m[11] = m.m[12] = m.m[13] = m.m[14] = 0.0f;
	glMultMatrixf(m.m);
}

void CraftCamera::SetDis(float delta)
{
	m_curDis += delta;
	if (m_curDis < MIN_DIS)
		m_curDis = MIN_DIS;
	if (m_curDis > MAX_DIS)
		m_curDis = MAX_DIS;
}

void CraftCamera::ChangeMode()
{
	if (m_changeModeCD < 0.f) {
		m_mode = 1 - m_mode;
		m_changeModeCD = CHANGDE_MODE_CD;
	}
}

Vec4f CraftCamera::GetEye2At()
{
	return lookMat.GetEye2At();
}

Vec4f CraftCamera::GetUp()
{
	return lookMat.GetUp();
}

CraftCamera* CraftCamera::create(_In_ Craft* craft)
{
	CraftCamera* ret = new CraftCamera(craft);
	ret->AddRef();
	return ret;
}
