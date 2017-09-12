/************************************************************
[概要]
Design Parameterなど、大量のデータを格納する際、
Globalに置いたclass 配列をDesignParameterで初期化して使用したい時がある。

この時、配列 各要素の初期化にコンストラクタを使用すると、
見通しが良くなって便利だが、
コピーコンストラクタ周りの動作で疑問が生じたので、
テストし、内容をまとめておく。


[まとめ]
・	配列要素(Global)をコンストラクタで初期化するcodeを書いた場合、
		compile時:
			コンストラクタを使って、temporary変数を作成し、コピーコンストラクタで配列の要素を初期化
		動作時:
			コピーコンストラクタを通らず、直接要素の初期化が行われている。
			配列が定義された時に「引数なしコンストラクタ」が呼ばれることもない。
		
	と動いている様子。
	例えばコピーコンストラクタをprivateにしてコピー禁止classにすると、compileが通らなかった。
	動作時は通らないのに、何故こうなるのかは不明のまま。
		
	
・	コピーコンストラクタは、以下の形式で定義(constも忘れずに).
		T(const T&org){}
		
	これがないとcompileが通らないが、動作時は、ここを通っていない.
	詳細は不明のまま.
************************************************************/





/************************************************************
************************************************************/
#include "stdio.h"
#include <new>

/************************************************************
************************************************************/
#define CASE 5

/************************************************************
構造体スタイルの初期化。
************************************************************/
#if (CASE == 0)
	class PARAM{
	private:
	public:
		int a;
	};
	
	PARAM param[] = {
		{109},
	};
	
	int main()
	{
		printf("%d\n", param[0].a);
		
		return 0;
	}

/************************************************************
初期化時の、{}スタイルとコンストラクタ・スタイルを比較。
どちらも実際は、コンストラクタを通っていた。
************************************************************/
#elif (CASE == 1)
	class PARAM{
	private:
	public:
		int a;
		
		PARAM(int _a)
		: a(_a)
		{
			printf("c0\n");
		}
	};
	
	PARAM param[] = {
		// {109},
		PARAM(109),
	};
	
	int main()
	{
		printf("%d\n", param[0].a);
		
		return 0;
	}
	
/************************************************************
コピーコンストラクタをprivateにしてcopy禁止クラスにしてみた。

配列の初期化で、
{}スタイルは、問題なし。
PARAM(109), の方は、compile Error.
************************************************************/
#elif (CASE == 2)
	class PARAM{
	private:
		PARAM(const PARAM& org)
		{
			printf("c1\n");
		}
	public:
		int a;
		
		PARAM(int _a)
		: a(_a)
		{
			printf("c0\n");
		}
		
		void operator =(const PARAM& org)
		{
			a = org.a;
		}
	};
	
	PARAM param[] = {
		// {109}, // ok.
		PARAM(109), // compile Error.
	};
	
	int main()
	{
		printf("%d\n", param[0].a);
		
		return 0;
	}
	
/************************************************************
配列の初期化で、
	PARAM(109),
などとした時は、temporary変数を作成し、これを使ってコピーコンストラクタで配列に入れているのかと思い、
コピーコンストラクタにprintfを入れてテスト。

しかし、動作時は、ここを通っていなかった。
************************************************************/
#elif (CASE == 3)
	class PARAM{
	private:
	public:
		int a;
		
		PARAM(int _a)
		: a(_a)
		{
			printf("c0\n");
		}
		
		PARAM(const PARAM& org)
		: a(org.a)
		{
			printf("c1\n"); // 通らない.
		}
		
		void operator =(const PARAM& org)
		{
			printf("ope=\n");
			a = org.a;
		}
	};
	
	PARAM param[] = {
		PARAM(109),
		PARAM(119),
	};
	
	int main()
	{
		printf("%d\n", param[0].a);
		printf("%d\n", param[1].a);
		printf("%d\n", sizeof(param)/sizeof(param[0]));
		
		return 0;
	}
	
/************************************************************
ちなみに、コピーコンストラクタで、constを外すと、やはりcompile Error.
************************************************************/
#elif (CASE == 4)
	class PARAM{
	private:
	public:
		int a;
		
		PARAM(int _a)
		: a(_a)
		{
			printf("c0\n");
		}
		
		PARAM(PARAM& org)
		{
			printf("c1\n");
		}
	};
	
	PARAM param[] = {
		PARAM(109), // compile Error.
		// {109}, // ok.
	};
	
	int main()
	{
		printf("%d\n", param[0].a);
		
		return 0;
	}
	
/************************************************************
配列の初期化でコンストラクタを使用した場合と
placement newの動きを比較しようと思い、以下をテスト。

placement newの動きはわかり易いが、
やはり動きは違った。
************************************************************/
#elif (CASE == 5)
	class PARAM{
	private:
	public:
		int a;
		
		PARAM()
		{
			printf("c none\n");
		}
		
		PARAM(int _a)
		: a(_a)
		{
			printf("c0\n");
		}
		
		PARAM(const PARAM& org)
		: a(org.a)
		{
			printf("c1\n"); // 通らない.
		}
		
		void operator =(const PARAM& org)
		{
			printf("ope=\n");
			a = org.a;
		}
	};
	
	PARAM param[2];
	
	int main()
	{
		new( param + 0 ) PARAM( 109 );
		new( param + 1 ) PARAM( 119 );
		
		printf("%d\n", param[0].a);
		printf("%d\n", param[1].a);
		printf("%d\n", sizeof(param)/sizeof(param[0]));
		
		return 0;
	}
	
#endif




	
