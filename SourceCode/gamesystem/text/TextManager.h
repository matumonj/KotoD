#pragma once
#include"DirectXCommon.h"
#include"VariableCommon.h"
#include"Font.h"
#include <map>
using namespace std;         //  名前空間指定
class TextManager
{
private:
	struct Word {
		wchar_t* FirstWord;//ˆês–Ú
		wchar_t* SecondWord;//“ñs–Ú
		wchar_t* ThirdWord;//ŽOs–Ú
	};
	struct Conversation
	{
		Font* FirstFont;
		Font* SecondFont;
		Font* ThirdFont;
	};

public:
	enum Name
	{
		NONE = 0,
		TYUTORIAL_TALK1,
		TYUTORIAL_TALK2,
		TYUTORIAL_TALK3,
		TYUTORIAL_TALK4,
		TYUTORIAL_TALK5,
		TYUTORIAL_TALK6,
		TYUTORIAL_TALK7,
		TYUTORIAL_TALK8,
		TYUTORIAL_TALK9,
		TYUTORIAL_TALK10,
		TYUTORIAL_TALK11,
		TYUTORIAL_TALK12,
		TYUTORIAL_TALK13,
		TYUTORIAL_TALK14,
		TYUTORIAL_TALK15,
		TYUTORIAL_TALK16,
		TYUTORIAL_TALK17,
		TYUTORIAL_TALK18,
		TYUTORIAL_TALK19,
		TYUTORIAL_TALK20,
		TYUTORIAL_TALK21,
		TYUTORIAL_TALK22,
		TYUTORIAL_TALK23,
		TYUTORIAL_TALK24,
		TYUTORIAL_TALK25,
		TYUTORIAL_TALK26,
		TYUTORIAL_TALK27,
		TYUTORIAL_TALK28,
		TYUTORIAL_TALK29,
		TYUTORIAL_TALK30,
		TYUTORIAL_TALK31,
		TYUTORIAL_TALK32,
		TYUTORIAL_TALK33,
		AISATU,
		SETUMEI1,
		SETUMEI2,
		SETUMEI3,
		SETUMEI4,
	};

	enum Name_Cap
	{
		NO,
		KOTO1,
		SUTO1,
		CAP1,
		KOTO2,
		SUTO2,
		CAP2,
		KOTO3,
		SUTO3,
		CAP3,
	};

	enum Name_First
	{
		VIEWBOSS,
		SPEAKPLAYER1,
		SPEALPLAYER2,
		SPEALPLAYER3,

		CHARGE1,
		CHARGE2,
		CHARGE3,
		CHARGE4,
	};

	enum Name_Second {
		ANGER_TALK,
		ANGER_TALK2,
		JOY_TALK,
		JOY_TALK2,
		JOY_TALK3,
		SELECT_TALK,
		SELECT_ANGER,
		SELECT_ANGER2,
		SELECT_JOY,
		SELECT_JOY2,
	};

	enum Name_Fourth {
		TALK_FIRST,
		TALK_SECOND,
		TALK_THIRD,
		TALK_FOURTH,
		TALK_FIVE,
	};

	enum Name_CameraBoss {
		TALK_FIRST_T,
		TALK_SECOND_T,
		TALK_THIRD_T,
		TALK_FOURTH_T,
		TALK_FIVE_T,
		TALK_SIX_T,
		TALK_SEVEN_T,
		TALK_EIGHT_T,
		TALK_NINE_T,
		TALK_TEN_T,
		TALK_ELEVEN_T,
		TALK_XII_T,
		TALK_XIII_T,
		TALK_XIV_T,
		TALK_XV_T,
		TALK_XVI_T,
		TALK_XVII_T,
		TALK_XVIII_T,
		TALK_XVIV_T,
		TALK_NULL,
	};

	enum Name_Last {
		LAST_TALK_FIRST,
		LAST_TALK_SECOND,
		LAST_TALK_THIRD,
		LAST_TALK_FOURTH,
		LAST_TALK_FIVE,
		LAST_TALK_SIX,
		LAST_TALK_SEVEN,
		LAST_TALK_EIGHT,
		LAST_TALK_NINE,
		LAST_TALK_TEN,
		LAST_TALK_ELEVEN,
		LAST_TALK_TWELVE,
		LAST_TALK_THIRTEEN,
		LAST_TALK_FOURTEEN,
		LAST_TALK_FIFETEEN,
		LAST_TALK_SIXTEEN,
		LAST_TALK_SEVENTEEN,
		LAST_TALK_EIGHTTEEN,
		LAST_TALK_NINETEEN,
		LAST_TALK_TWENTY,
		LAST_TALK_TWENTYONE,
		LAST_TALK_TWENTYSECOND,
		AWAKE_FIRST,
		AWAKE_SECOND,
		AWAKE_THIRD,
		DEATH_FIRST,
		DEATH_SECOND,
		DEATH_THIRD,
		DEATH_FOURTH,
		DEATH_FIVE,
		DEATH_SIX,
		DEATH_SEVEN,
		DEATH_EIGHT,
		DEATH_NINE,
		DEATH_TEN,
		DEATH_ELEVEN,
		DEATH_TWELVE,
		DEATH_THIRTEEN,
		DEATH_FOURTEEN,
		DEATH_FIFTEEN,
	};

	static TextManager* GetInstance();

	//
	void Create(DirectXCommon* dxcomon);

	void Initialize(DirectXCommon* dxcomon);

	void Draw(DirectXCommon* dxcommon);

	void TestDraw(DirectXCommon* dxcommon);

	void Test();

	void SetAllColor(const XMVECTOR& color={1.f,1.f,1.f,1.f});

	void SetOnceColor(int row, const XMVECTOR& color = { 1.f,1.f,1.f,1.f });

	void SetConversation(Name name=NONE,const XMVECTOR& color={1.f,1.f,1.f,1.f});

	void NoneText();

	void SetConversation(Name_First name = VIEWBOSS, const XMVECTOR& color = { 1.f,1.f,1.f,1.f });

	void SetSecondConversation(Name_Second name = ANGER_TALK);

	void SetFourthConversation(Name_Fourth name = TALK_FIRST);

	void SetLastConversation(Name_Last name = LAST_TALK_FIRST);

	void SetCameraBossConversation(Name_CameraBoss name = TALK_FIRST_T);

	void SetCapConversation(Name_Cap name = CAP1);

	void GetWordSize(Word word);

	void SetRowPosition(float posX);
private:
	
	//
	void CreateWord(Name name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	void CreateWord(Name_First name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	void SecondCreateWord(Name_Second name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	void FourthCreateWord(Name_Fourth name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	void LastCreateWord(Name_Last name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	void CameraBossCreateWord(Name_CameraBoss name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	void CreateCapWord(Name_Cap name, wchar_t* tex1, wchar_t* tex2 = L" ", wchar_t* tex3 = L" ");
	//
	Word SetWord(wchar_t* tex1, wchar_t* tex2, wchar_t* tex3);
	//
	Conversation CreateConversation(Word word);

	void CreateCon(Conversation con, Word word);
private:
	std::map<TextManager::Name, Word> wordlist_;
	std::map<TextManager::Name_First, Word> wordlist_first;
	std::map<TextManager::Name_Second, Word> wordlist_second;
	std::map<TextManager::Name_CameraBoss, Word> wordlist_cameraBoss;
	std::map<TextManager::Name_Fourth, Word> wordlist_fourth;
	std::map<TextManager::Name_Last, Word> wordlist_last;
	std::map<TextManager::Name_Cap, Word> wordlist_cap;

	Conversation conversation_ = {};
	Conversation old_conversation_ = {};

	Conversation bossconversation_ = {};

	XMVECTOR color_{ 1.f,1.f,1.f,1.f };
	size_t len[3];
	bool flag[3] = { true,true,true };
	bool next_f[3] = { false,false,false};

	bool testF = false;
	float time_ = 0.f;
	int length = 0;
	wchar_t* test;
	wchar_t* test1;
	wchar_t* test2;

	Name old=NONE;
	Name_First old_first = VIEWBOSS;
	Name_Second old_second = ANGER_TALK;
	Name_Fourth old_fourth = TALK_FIRST;
	Name_CameraBoss old_cameraBoss = TALK_FIRST_T;
	Name_Last old_last = LAST_TALK_FIRST;
	Name_Cap old_cap = CAP1;
};

