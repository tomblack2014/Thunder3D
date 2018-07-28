#include "Boss.h"
#include "global.h"

#include <gl\GL.h>
#include <gl\GLU.h>
#include <random>
using namespace std;
//临时措施：提供随机数生成源
static default_random_engine re(GetTickCount());

const double PI = 3.141592653;

Boss::~Boss()
{
}


void Boss::Frame(_In_ float interval)
{
	for (auto iter : m_bullets) {
		iter->Frame(interval);
	}

	//第一阶段
	if (m_status == 0) {
		Status1(interval);
	}
	//第二阶段
	else if (m_status == 1) {
		Status2(interval);
	}
	//第三阶段
	else if (m_status == 2) {
		Status3(interval);
	}

	//子弹生命周期判断
	for (auto iter = m_bullets.begin(); iter != m_bullets.end();) {
		if ((*iter)->GetLifeTime() < 0) {
			SafeRelease(&(*iter));
			iter = m_bullets.erase(iter);
			continue;
		}
		iter++;
	}
}

void Boss::Status1(float interval)
{
	//位置缓缓漂移
	auto pos = m_craft->GetPos();
	if ((pos + m_v * interval).length() > 5.f)
		RanV();
	else
		m_craft->SetPos(pos + m_v * interval);

	//模式一
	if (m_subStatus == 0) {
		//每个状态开始都先cd一下
		if (m_cdTime < 2.f) {
			m_cdTime += interval;
		}
		else {
			//追踪和锁定阶段都旋转朝向player
			if (m_flag[0][0] == 0 || m_flag[0][0] == 1) {
				const float ROT_SPEED = 0.6f;
				Vec4f moveTo = m_playerPos - GetPos();
				Vec4f v = m_craft->GetSpeed();
				float ang = moveTo.CalAng(v);
				if (fabs(ang) > 0.01f) {
					float rAng = interval * ROT_SPEED;
					Vec4f axis = moveTo.cross(v);
					//小于阈值开始发射线
					if (fabs(ang) < 0.2f) {
						m_flag[0][0] = 1;
					}
					if (fabs(ang) < rAng)
						rAng = fabs(ang);
					m_craft->Rotate(axis, -rAng);
				}
				m_craft->Rotate(v, interval * ROT_SPEED);
				m_lastPPos = m_playerPos;
			}
			//锁定阶段放出射线
			if (m_flag[0][0] == 1) {
				DrawLockBeam();
				m_timer[0][0] += interval;
				if (m_timer[0][0] > 2.f) {
					m_flag[0][0] = 2;
				}
			}
			//停1s(射线不再追踪)
			if (m_flag[0][0] == 2) {
				m_timer[0][1] += interval;
				if (m_timer[0][1] > 2.f) {
					m_flag[0][0] = 3;
					m_shootCD[0][0] = 0.2f;
					m_flag[0][1] = 0;
				}
			}
			//射出大量子弹
			if (m_flag[0][0] == 3) {
				if (m_shootCD[0][0] < 0.f) {
					m_shootCD[0][0] = 0.5f;
					auto v = m_craft->GetSpeed().normal();
					auto up = m_craft->GetUp().normal();
					auto right = up.cross(v);
					//正方形A
					if (m_flag[0][1] == 0) {
						RectShoot(up, right, v);
					}
					//正方形B
					else {
						Shoot(pos + up * 0.7f, v * 4, 4.f);
						Shoot(pos + right * 0.7f, v * 4, 4.f);
						Shoot(pos - right * 0.7f, v * 4, 4.f);
						Shoot(pos - up * 0.7f, v * 4, 4.f);
					}
					m_flag[0][1] = 1 - m_flag[0][1];
				}

				m_shootCD[0][0] -= interval;
				m_timer[0][2] += interval;
				if (m_timer[0][2] > 5.f) {
					memset(m_timer[0], 0, sizeof(m_timer[0]));
					memset(m_flag[0], 0, sizeof(m_flag[0]));
					m_shootCD[0][0] = 0.f;
					m_subStatus = 1;
					m_cdTime = 0.f;
					m_frameCount1_2 = 0;
				}
			}
		}
	}
	//模式二
	else if (m_subStatus == 1) {
		//每个状态开始都先cd一下
		if (m_cdTime < 1.f) {
			m_cdTime += interval;
		}
		else {
			if (m_flag[0][2] == 0) {
				const float ROT_SPEED = 0.6f;
				Vec4f moveTo = m_playerPos - GetPos();
				Vec4f v = m_craft->GetSpeed();
				float ang = moveTo.CalAng(v);
				if (fabs(ang) > 0.01f) {
					float rAng = interval * ROT_SPEED;
					Vec4f axis = moveTo.cross(v);
					//小于阈值开始发射线
					if (fabs(ang) < 0.2f) {
						m_flag[0][2] = 1;
					}
					if (fabs(ang) < rAng)
						rAng = fabs(ang);
					m_craft->Rotate(axis, -rAng);
				}
				m_craft->Rotate(v, interval * ROT_SPEED);
			}
			if (m_flag[0][2] == 1) {
				if (m_shootCD[0][1] < 0.f) {
					m_shootCD[0][1] = 0.2f;
					auto v = m_craft->GetSpeed().normal();
					auto up = m_craft->GetUp().normal();
					auto right = up.cross(v);
					//十字斩，每次向外扩散一层
					auto pos = m_craft->GetPos();
					Shoot(pos + up * sqrt(m_frameCount1_2) * 0.7f, v * 4, 4.f);
					Shoot(pos + right * sqrt(m_frameCount1_2) * 0.7f, v * 4, 4.f);
					Shoot(pos - right * sqrt(m_frameCount1_2) * 0.7f, v * 4, 4.f);
					Shoot(pos - up * sqrt(m_frameCount1_2) * 0.7f, v * 4, 4.f);
					m_frameCount1_2 += 8;
				}
				m_shootCD[0][1] -= interval;
				m_timer[0][3] += interval;
				if (m_timer[0][3] > 2.f) {
					memset(m_timer[0], 0, sizeof(m_timer[0]));
					memset(m_flag[0], 0, sizeof(m_flag[0]));
					m_shootCD[0][0] = 0.f;
					m_subStatus = 0;
					m_cdTime = 0.f;
				}
			}
		}
	}

	//判断血量跳转阶段
	if (m_curHP <= m_MaxHP * 0.5f) {
		m_subStatus = 0;
		m_status = 1;
		memset(m_timer[1], 0, sizeof(m_timer[0]));
		memset(m_flag[1], 0, sizeof(m_flag[0]));
		m_subStatus = 0;
		m_cdTime = 0.f;
		m_movePosX = m_movePosY = 1;
	}
}

void Boss::Status2(float interval)
{
	//瞬移
	if (m_subStatus == 0) {
		//每个状态开始都先cd一下
		if (m_cdTime < 2.f) {
			m_cdTime += interval;
		}
		else {
			//另外八个落地点随机一个
			if (m_flag[1][0] == 0) {
				int x, y, z;
				do {
					x = uniform_int_distribution<int>(0, 2)(re);
					y = uniform_int_distribution<int>(0, 2)(re);
					z = uniform_int_distribution<int>(0, 2)(re);
					if (m_movePosX != x || m_movePosY != y || m_movePosZ != z) {
						m_movePosX = x;
						m_movePosY = y;
						m_movePosZ = z;
						break;
					}
				} while (true);
				m_flag[1][0] = 1;
			}
			//在落地点画提示物体
			if (m_flag[1][0] == 1) {
				m_timer[1][0] += interval;
				if (m_timer[1][0] > 5.f)
					m_flag[1][0] = 2;
				else
					DrawPyramin(m_timer[1][0]);
			}
			//移动
			if (m_flag[1][0] == 2) {
				m_craft->SetPos(Vec4f(
					(m_movePosX - 1) * 5.f,
					(m_movePosY - 1) * 5.f,
					(m_movePosZ - 1) * 5.f));
				m_flag[1][0] = 0;
				m_timer[1][0] = 0.f;
				m_shootCD[1][0] = 0.f;
				m_subStatus = 1;
				m_cdTime = 0.f;
				m_shootAng2_1 = 10.f * 2 * PI / 360;
			}
		}
	}
	//模式一
	else if (m_subStatus == 1) {
		//每个状态开始都先cd一下
		if (m_cdTime < 2.f) {
			m_cdTime += interval;
		}
		else {
			//追踪和锁定阶段都旋转朝向player
			if (m_flag[1][1] == 0 || m_flag[1][1] == 1) {
				const float ROT_SPEED = 0.6f;
				Vec4f moveTo = m_playerPos - GetPos();
				Vec4f v = m_craft->GetSpeed();
				float ang = moveTo.CalAng(v);
				if (fabs(ang) > 0.01f) {
					float rAng = interval * ROT_SPEED;
					Vec4f axis = moveTo.cross(v);
					//小于阈值开始发射线
					if (fabs(ang) < 0.2f) {
						m_flag[1][1] = 1;
					}
					if (fabs(ang) < rAng)
						rAng = fabs(ang);
					m_craft->Rotate(axis, -rAng);
				}
				m_craft->Rotate(v, interval * ROT_SPEED);
				m_lastPPos = m_playerPos;
			}
			//锁定阶段放出射线
			if (m_flag[1][1] == 1) {
				DrawLockBeam();
				m_timer[0][0] += interval;
				if (m_timer[0][0] > 2.f) {
					m_flag[1][1] = 2;
				}
			}
			//停1s(射线不再追踪)
			if (m_flag[1][1] == 2) {
				m_timer[1][1] += interval;
				if (m_timer[1][1] > 2.f) {
					m_flag[1][1] = 3;
					m_shootCD[1][0] = 0.2f;
					m_flag[1][2] = 0;
				}
			}
			//射出大量子弹TODO
			if (m_flag[1][1] == 3) {
				if (m_shootCD[1][0] < 0.f) {
					m_shootCD[1][0] = 0.5f;
					auto v = m_craft->GetSpeed().normal();
					auto up = m_craft->GetUp().normal();
					auto right = up.cross(v);

					auto tV = Mat4f::rotate(up, m_shootAng2_1) * v;
					auto tUp = up;
					auto tRight = tV.cross(tUp);
					RectShoot(tUp, tRight, tV);

					tV = Mat4f::rotate(up, -m_shootAng2_1) * v;
					tUp = up;
					tRight = tV.cross(tUp);
					RectShoot(tUp, tRight, tV);

					tV = Mat4f::rotate(right, m_shootAng2_1) * v;
					tRight = right;
					tUp = tRight.cross(tV);
					RectShoot(tUp, tRight, tV);

					tV = Mat4f::rotate(right, -m_shootAng2_1) * v;
					tRight = right;
					tUp = tRight.cross(tV);
					RectShoot(tUp, tRight, tV);
				}

				m_shootAng2_1 += interval * 0.2;
				m_shootCD[1][0] -= interval;
				m_timer[1][2] += interval;
				if (m_timer[1][2] > 5.f) {
					memset(m_timer[1], 0, sizeof(m_timer[1]));
					memset(m_flag[1], 0, sizeof(m_flag[1]));
					m_shootCD[1][0] = 0.f;
					m_subStatus = 2;
					m_cdTime = 0.f;
				}
			}
		}
	}
	//模式二:乱舞
	else if (m_subStatus == 2) {
		//每个状态开始都先cd一下
		if (m_cdTime < 2.f) {
			m_cdTime += interval;
		}
		else {
			const float ROT_SPEED = 1.5f;
			if (m_flag[1][2] == 0) {
				float x = uniform_real_distribution<float>(-1.f, 1.f)(re);
				float y = uniform_real_distribution<float>(-1.f, 1.f)(re);
				float z = uniform_real_distribution<float>(-1.f, 1.f)(re);
				m_tarVec2_2 = Vec4f(x, y, z).normal();
				m_flag[1][2] = 1;
			}
			else {
				if (m_craft->GetSpeed().CalAng(m_tarVec2_2) < 0.2f) {
					float x = uniform_real_distribution<float>(-1.f, 1.f)(re);
					float y = uniform_real_distribution<float>(-1.f, 1.f)(re);
					float z = uniform_real_distribution<float>(-1.f, 1.f)(re);
					m_tarVec2_2 = Vec4f(x, y, z).normal();
				}
				else {
					Vec4f axis = m_craft->GetSpeed().cross(m_tarVec2_2);
					m_craft->Rotate(axis, ROT_SPEED * interval);
					if (m_shootCD[1][1] < 0.f) {
						m_shootCD[1][1] = 0.1f;
						Vec4f v = m_craft->GetSpeed().normal();
						Vec4f up = m_craft->GetUp().normal();
						Vec4f right = v.cross(up);
						Shoot(m_craft->GetPos(), v * 4, 4.f);
						Shoot(m_craft->GetPos(), -v * 4, 4.f);
						Shoot(m_craft->GetPos(), up * 4, 4.f);
						Shoot(m_craft->GetPos(), -up * 4, 4.f);
						Shoot(m_craft->GetPos(), right * 4, 4.f);
						Shoot(m_craft->GetPos(), -right * 4, 4.f);
						
					}
				}
			}
			m_shootCD[1][1] -= interval;
			m_timer[1][3] += interval;
			if (m_timer[1][3] > 30.f) {
				memset(m_timer[1], 0, sizeof(m_timer[1]));
				memset(m_flag[1], 0, sizeof(m_flag[1]));
				m_subStatus = 0;
				m_cdTime = 0.f;
			}
		}
	}

	//判断血量跳转阶段
	if (m_curHP <= m_MaxHP * 0.1f) {
		m_subStatus = 0;
		m_status = 2;
	}
}

void Boss::Status3(float interval)
{
	//准备时间
	if (m_subStatus == 0) {
		if (m_cdTime < 4.f) {
			m_cdTime += interval;
		}
		else {
			m_cdTime = 0.f;
			m_subStatus = 1;
			m_shootCD[2][0] = 0.1f;
		}
	}
	//追踪+射击
	else if (m_subStatus == 1) {
		//一直向目标点旋转
		const float ROTATE_SPEED = 0.15f;
		Vec4f moveTo = m_playerPos - GetPos();
		Vec4f v = m_craft->GetSpeed();
		float ang = moveTo.CalAng(v);
		if (fabs(ang) > 0.01f) {
			float rAng = interval * ROTATE_SPEED;
			if (rAng > fabs(ang))
				rAng = ang;
			Vec4f axis = moveTo.cross(v);
			m_craft->Rotate(axis, -rAng);
		}
		m_craft->Rotate(v, interval * ROTATE_SPEED);

		const float SHOOT_CD = 0.5f;
		m_shootCD[2][0] -= interval;
		if (m_shootCD[2][0] < 0.f) {
			Shoot(m_craft->GetPos(), m_craft->GetSpeed().normal() * 4, 4.f);
		}
	}
}

void Boss::Render()
{
	m_craft->Render();
	for (auto iter : m_bullets) {
		iter->Render();
	}
}

Boss::Boss(Craft* craft, int hp) :
	BaseAI(craft, hp),
	m_status(1),
	m_subStatus(0),
	m_cdTime(0.f),
	m_lastPPos(Vec4f())
{
	m_craft->SetPos(Vec4f(0.f, 0.f, 0.f));
	RanV();
	memset(m_flag, 0, sizeof(m_flag));
	memset(m_timer, 0, sizeof(m_timer));
}

void Boss::RanV()
{
	float x = uniform_real_distribution<float>(0.0f, 0.2f)(re);
	float y = uniform_real_distribution<float>(0.0f, 0.2f)(re);
	float z = uniform_real_distribution<float>(0.0f, 0.2f)(re);

	m_v = Vec4f(x, y, z);
}

void Boss::RectShoot(Vec4f up, Vec4f right, Vec4f v)
{
	auto pos = m_craft->GetPos();
	Shoot(pos + up * 0.5f + right * 0.5f, v * 4, 4.f);
	Shoot(pos - up * 0.5f + right * 0.5f, v * 4, 4.f);
	Shoot(pos + up * 0.5f - right * 0.5f, v * 4, 4.f);
	Shoot(pos - up * 0.5f - right * 0.5f, v * 4, 4.f);
}

void Boss::DrawLockBeam()
{
	Vec4f pos = GetPos();
	glPushMatrix();
	glTranslatef(0.f, 0.f, 0.f);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(m_lastPPos.x, m_lastPPos.y, m_lastPPos.z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Boss::DrawPyramin(float time)
{
	glTranslatef(0, fabs(fabs(time - (int)time) - 0.5f), 0);

	Vec4f top((m_movePosX - 1) * 5.f, (m_movePosY - 1) * 5.f, (m_movePosZ - 1) * 5.f);
	Vec4f zVec(0, 1, 0);
	Vec4f bottom1 = top + zVec + Vec4f(0.5f, 0.f, 0.f);
	Vec4f bottom2 = top + zVec + Vec4f(0.f, 0.f, 0.5f);
	Vec4f bottom3 = top + zVec + Vec4f(-0.5f, 0.f, 0.f);
	Vec4f bottom4 = top + zVec + Vec4f(0.f, 0.f, -0.5f);

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(top.x, top.y, top.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom1.x, bottom1.y, bottom1.z);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bottom2.x, bottom2.y, bottom2.z);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(top.x, top.y, top.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom3.x, bottom3.y, bottom3.z);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bottom2.x, bottom2.y, bottom2.z);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(top.x, top.y, top.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom3.x, bottom3.y, bottom3.z);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bottom4.x, bottom4.y, bottom4.z);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(top.x, top.y, top.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom1.x, bottom1.y, bottom1.z);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bottom4.x, bottom4.y, bottom4.z);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bottom2.x, bottom2.y, bottom2.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom1.x, bottom1.y, bottom1.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom3.x, bottom3.y, bottom3.z);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom3.x, bottom3.y, bottom3.z);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(bottom1.x, bottom1.y, bottom1.z);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(bottom4.x, bottom4.y, bottom4.z);

	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Boss::Shoot(Vec4f sPos, Vec4f v, float lifetime)
{
	Bullet* bullet = Bullet::create(sPos, v, lifetime, 0.02f);
	bullet->Initialize("config/sphere.bin", "config/blue.bmp");
	m_bullets.push_back(bullet);
}

int Boss::Collide(_In_ const Vec4f& point, float radius, bool& isDead)
{
	int ret = 0;

	isDead = false;
	if (point.dist(m_craft->GetPos()) < T3D::MODEL_SIZE + radius) {
		ret += 100;
		isDead = true;
	}

	for (auto iter2 = m_bullets.begin(); iter2 != m_bullets.end();) {
		if (point.dist((*iter2)->GetPos()) < radius) {
			SafeRelease(&(*iter2));
			iter2 = m_bullets.erase(iter2);
			ret++;
			continue;
		}
		iter2++;
	}

	return ret;
}

bool Boss::Collide(_In_ const Vec4f& point)
{
	//模型大小预估值

	return point.dist(m_craft->GetPos()) < T3D::MODEL_SIZE;
}