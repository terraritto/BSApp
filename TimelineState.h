#pragma once
#include "StateBase.h"
#include "GlobalInfo.h"

struct Item
{
	String name;
	String text;

	// アイテムの描画
	void draw(const Vec2& basePos, const Font& font, int32 order) const
	{
		drawImpl(getRect(basePos, order), font, false);
	}

	// アイテムの長方形を返す
	RectF getRect(const Vec2& basePos, int32 order) const
	{
		return{ basePos.movedBy(0, (110 * order)), 400, 90 };
	}

private:

	void drawImpl(const RectF& rect, const Font& font, bool shadow) const
	{
		if (shadow)
		{
			rect.rounded(8).drawShadow(Vec2{ 2, 2 }, 16, 2).draw();
		}
		else
		{
			rect.rounded(8).draw();
		}

		// 解析しつつそれっぽく改行(力技)
		String result = text;
		int index = 0;
		int offset = 0;
		const int fixedNum = 25;
		while (true)
		{
			// 次の改行位置がサイズ内か？
			int pos = (index + 1) * fixedNum + index + offset;
			if (result.size() < pos)
			{
				break;
			}

			// そもそも改行指定ある？
			int newLinePos = result.indexOf(U"\n", index * fixedNum + index);
			if (newLinePos != -1 && newLinePos < pos)
			{
				offset = pos - newLinePos;
				index++;
				continue;
			}

			// 改行行挿入
			result.insert(pos, U"\n");
			index++;
		}

		Vec2 pos = Vec2{ rect.leftX(), rect.topY() };
		font(name).draw(15, pos.movedBy(10, 0), ColorF{0.11});
		font(result).draw(15, pos.movedBy(10, 20), ColorF{ 0.11 });
	}
};


class TimelineState : public StateBase
{
public:
	enum class TimelineProcess
	{
		Initialize,
		Timeline
	};

	TimelineState() : StateBase() { m_state = State::Timeline; m_process = TimelineProcess::Initialize; }

	void Update() override
	{
		switch (m_process)
		{
		case TimelineProcess::Initialize:
			{
				m_itemList.resize(MaxTimeline);

				UpdateData();

				// 最初はタイマー待ち
				while (true)
				{
					if (m_updateTimer.reachedZero())
					{
						m_updateTimer.reset();
						break;
					}
				}

				// jsonの読み込みとデータ生成
				LoadJson();

				m_process = TimelineProcess::Timeline;
			}
			break;

		case TimelineProcess::Timeline:
			{
				const Point basePos{ 80, 80 };
				// リストの背景を描画する
				RectF{ basePos, 400, 600 }.stretched(24).rounded(8).draw(ColorF{ 0.9 });

				// リスト上のアイテムを描画する
				{
					int32 order = 0;

					for (const auto& item : m_itemList)
					{
						item.draw(basePos, m_font, order);

						++order;
					}
				}

				// pos
				SimpleGUI::TextBox(m_postText, Vec2{ 10, 10 }, 600);

				// ボタンが押されたらポストして更新
				if (SimpleGUI::Button(U"Post", Vec2{ 620, 10 }))
				{
					// ポスト
					PostData();
					Timer postTime{ 1.0s };
					postTime.start();
					while (true)
					{
						if (postTime.reachedZero())
						{
							break;
						}
					}

					// 更新
					UpdateData();
					m_updateTimer.restart();
				}

				// 単純に更新
				if (SimpleGUI::Button(U"Update", Vec2{ 620, 60 }))
				{
					UpdateData();
					m_updateTimer.restart();
				}

				// timer処理
				if (m_updateTimer.reachedZero())
				{
					// 一定時間経過した段階でJsonから読み出す
					m_updateTimer.reset();
					LoadJson();
				}

				// クッキークリッカー(何故)
				CookieClick();
			}
			break;
		}
	}

protected:
	void UpdateData()
	{
		// 読み込み開始
		String loginCommand;
		auto dir = FileSystem::CurrentDirectory() + U"py/Timeline.bat";
		loginCommand = U" " + ID + U" " + Pass;
		dir = U"/c" + dir + loginCommand; // コマンドと結合していく
		ShellExecute(NULL, L"open", L"cmd.exe", dir.toWstr().c_str(), L"", SW_HIDE);

		m_updateTimer.set(5s);
		m_updateTimer.start();
	}

	void PostData()
	{
		// 読み込み開始
		String loginCommand;
		auto dir = FileSystem::CurrentDirectory() + U"py/Post.bat";
		loginCommand = U" " + ID + U" " + Pass + U" " + m_postText.text;
		dir = U"/c" + dir + loginCommand; // コマンドと結合していく
		ShellExecute(NULL, L"open", L"cmd.exe", dir.toWstr().c_str(), L"", SW_HIDE);

		m_updateTimer.set(5s);
		m_updateTimer.start();
	}

	void LoadJson()
	{
		const String jsonPath = U"py/TimelineInfo.json";
		const JSON json = JSON::Load(jsonPath);

		// 失敗
		if (not json)
		{
			throw Error{ U"Failed to load: " + jsonPath };
		}

		String TimelineItem = U"timeline_";
		for (int i = 0; i < MaxTimeline; i++)
		{
			m_itemList[i].name = json[TimelineItem + U"{}"_fmt(i)][U"name"].getString();
			m_itemList[i].text = json[TimelineItem + U"{}"_fmt(i)][U"text"].getString();
		}
	}

	void CookieClick()
	{
		if (m_cookieCircle.mouseOver())
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		if (m_cookieCircle.leftClicked())
		{
			++m_cookies;
		}

		m_font(U"{:.0f}"_fmt(m_cookies)).drawAt(60 ,590, 320);
		m_texture.scaled(1.5).drawAt(m_cookieCircle.center);
	}

protected:
	Font m_font{ FontMethod::MSDF, 48 };

	TextEditState m_postText;

	TimelineProcess m_process;
	Array<Item> m_itemList;

	const int MaxTimeline = 5;

	Timer m_updateTimer;

	const Texture m_texture{ U"🍪"_emoji };
	const Circle m_cookieCircle{ 700, 450, 100 };
	double m_cookies = 0;
};
