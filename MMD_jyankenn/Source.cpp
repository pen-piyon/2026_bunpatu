#include "DxLib.h"

int play_MMD(int);

int play_MMD(int play_MMD_Number) {

    ChangeWindowMode(FALSE);

    WaitTimer(100);

    int MovieGraphHandle;
    int GSizeX, GSizeY;

    if (play_MMD_Number == 1) {
        // ムービーファイルをロードします。
        MovieGraphHandle = LoadGraph("MMD_file/stone-Trim.mp4");
    }
    else if (play_MMD_Number == 2) {
        // ムービーファイルをロードします。
        MovieGraphHandle = LoadGraph("MMD_file/scissors-Trim.mp4");
    }
    else if (play_MMD_Number == 3) {
        // ムービーファイルをロードします。
        MovieGraphHandle = LoadGraph("MMD_file/paper-Trim.mp4");
    }
    else {
        DxLib_End();
        return (0);
    }

    GetGraphSize(MovieGraphHandle, &GSizeX, &GSizeY);

    // ムービーを再生状態にします
    PlayMovieToGraph(MovieGraphHandle);

    // ループ、GetMovieStateToGraph 関数はムービーの再生状態を得る関数です
    // 戻り値が１の間は再生状態ですのでループを続けます
    while (ProcessMessage() == 0 && GetMovieStateToGraph(MovieGraphHandle) == 1) {
        // ムービー映像を画面いっぱいに描画します
        //DrawExtendGraph(-200, 0, GSizeX/3, GSizeY/3, MovieGraphHandle, FALSE);
        DrawExtendGraph(-590, -100, GSizeX / 2, GSizeY / 2, MovieGraphHandle, FALSE);
        // ウエイトをかけます、あまり速く描画すると画面がちらつくからです
        WaitTimer(17);
    }

    // 読み込んだムービーファイルのグラフィックハンドルの削除
    DeleteGraph(MovieGraphHandle);

    return (0);
}

//セーブデータの構造体
typedef struct {
    int Day_save;   // 日
    int Hour_save;  // 時間
    int Min_save;   // 分
    int Sec_save;   // 秒
    char playername_save[10];
    int rennsyouruu_save;
} SaveData;

//ランキング
typedef struct {
    int Day_ranking;   // 日
    int Hour_ranking;  // 時間
    int Min_ranking;   // 分
    int Sec_ranking;   // 秒
    char playername_ranking[10];
    int rennsyouruu_ranking;
} RankingData;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int end = 0,re=0;
    
    // 無限ループ用
//    while (end == 0)


        // ウィンドウのタイトルを変更
        SetMainWindowText("初音ミクとじゃんけん!!");
        ChangeWindowMode(TRUE);

        // 変数の定義
        int MMD_number = 0, number_NPC, rand_max_value = 300, win_draw_lose_hantei = 0;
        int i, rennsyousuu = 0, for_kyoka = 1;
        int ranking_max = 0;
        char player_jyannkenn_te[10], npc_jyannkenn_te[10], username[15];
        char player_te = 'a', npc_te = 'a';


        // 時間データの構造体
        DATEDATA Date;

        SaveData Savedata;

        RankingData rankingdata[10];

        // ＤＸライブラリ初期化処理
        if (DxLib_Init() == -1) {
            return -1;  // エラーが起きたら直ちに終了
        }

        // タイトル画面の表示

        // 描画する文字列のサイズを設定
        SetFontSize(60);
        DrawString(0, 30, "初音ミクとじゃんけん", GetColor(255, 255, 255));
        SetFontSize(15);
        DrawString(0, 100, "エンターキーを押してね", GetColor(255, 255, 255));
        WaitKey();
        // 画面の初期化
        ClearDrawScreen();

        //　・ゲーム開始前の処理

        //配列の初期化
        for (i = 0; i < 10; i++) {
            npc_jyannkenn_te[i] = '\0';
            player_jyannkenn_te[i] = '\0';
        }

        for (i = 0; i < 15; i++) {
            username[i] = '\0';
        }

        int t = 0;
        for (i = 0; i < 10; i++) {
            rankingdata[i].Day_ranking = 0;
            rankingdata[i].Hour_ranking = 0;
            rankingdata[i].Min_ranking = 0;
            rankingdata[i].Sec_ranking = 0;
            rankingdata[i].rennsyouruu_ranking = -1;

            for (t = 0; t < 10; t++) {
                rankingdata[i].playername_ranking[t] = '\0';
            }
        }


        // ファイルの作成
        // test.cpp ファイルを開く
        int FileHandle;

        FileHandle = FileRead_open("ranking.txt");

        // 一行読む
        // FileRead_gets(rankingdata_001.Day_ranking , 256, FileHandle);
        DrawFormatString(0, 80, GetColor(255, 255, 255), "何日 何時:何分,何秒 ユーザー名 連勝数 \n");
        for (i = 0; i < 10; i++) {

            if (FileRead_eof(FileHandle) != 0) {
                ranking_max = i;

                if (ranking_max == 0) {
                    ranking_max = 0;
                }

                break;
            }

            FileRead_scanf(FileHandle, "%d,%d,%d,%d,%[^,],%d",
                &rankingdata[i].Day_ranking,
                &rankingdata[i].Hour_ranking,
                &rankingdata[i].Min_ranking,
                &rankingdata[i].Sec_ranking,
                &rankingdata[i].playername_ranking,
                &rankingdata[i].rennsyouruu_ranking);

            DrawFormatString(0, 80 + 20 * (i + 1), GetColor(255, 255, 255), "%d %d:%d:%d %s 連勝数 %d回\n",
                rankingdata[i].Day_ranking,
                rankingdata[i].Hour_ranking,
                rankingdata[i].Min_ranking,
                rankingdata[i].Sec_ranking,
                rankingdata[i].playername_ranking,
                rankingdata[i].rennsyouruu_ranking);  //出力
        }

        //ユーザーネームの入力画面
        DrawString(0, 30, "ユーザーネームを入力してください( 10文字、半角英数字)", GetColor(255, 255, 255));
        // プレイヤーのじゃんけん入力
        KeyInputSingleCharString(0, 50, 30, username, FALSE);

        //繰り返しのfor文(ゲームスタート)
        while (for_kyoka == 1) 
        {
            for_kyoka = 0;

            // 画面の初期化
            ClearDrawScreen();
            re = 0;

            while (re == 0)
            {
                // じゃんけん入力指示文字列の描画
                DrawFormatString(0, 0, GetColor(255, 255, 255), "%d 回目の勝負です", rennsyousuu + 1);
                DrawString(0, 30, "あなたの出す手を入力して、エンターキーを押してください。\nグー → g\nチョキ → c\nパー → p\nEXIT → e\n管理者用 → a", GetColor(255, 255, 255));

                // プレイヤーのじゃんけん入力
                KeyInputSingleCharString(0, 150, 100, player_jyannkenn_te, FALSE);

                // 画面の初期化
                ClearDrawScreen();

                // EXITかを確認
                if (player_jyannkenn_te[0] == 'e') {
                    WaitTimer(500);
                    re = 2;
                }else if (player_jyannkenn_te[0] == 'a') {
                    end = 1;
                    WaitTimer(500);
                    re = 1;
                    DxLib_End();  // ＤＸライブラリ使用の終了処理
                    return 0;     // ソフトの終了

                }else if (player_jyannkenn_te[0] == 'g' || player_jyannkenn_te[0] == 'c' || player_jyannkenn_te[0] == 'p')
                {
                    re = 1;
                }
                else if (player_jyannkenn_te[0] == 'w')
                {
                    re = 1;
                }
                else
                {
                    re = 0;
                }

            }

            // 入力したじゃんけんの手を表示
            for (i = 0; i < 10; i++) {
                if (username[i] == '\0') {
                    break;
                }
                else {
                    DrawFormatString(10 * i, 0, GetColor(255, 255, 255), "%c", username[i]);
                }
            }
            DrawFormatString(105, 0, GetColor(255, 255, 255), "の手は");
            DrawString(0, 16, player_jyannkenn_te, GetColor(255, 255, 255));


            WaitTimer(3000);
            // キー入力待ち
            //WaitKey();

            // 画面の初期化
            ClearDrawScreen();

            //コンピュータ側のじゃんけんの手
            number_NPC = GetRand(rand_max_value);

            if (number_NPC >= 0 && number_NPC < rand_max_value / 3) {
                npc_jyannkenn_te[0] = 'g';
                MMD_number = 1;
            }
            else if (number_NPC >= rand_max_value / 3 && number_NPC < rand_max_value * 2 / 3) {
                npc_jyannkenn_te[0] = 'c';
                MMD_number = 2;
            }
            else if (number_NPC >= rand_max_value * 2 / 3 && number_NPC <= rand_max_value) {
                npc_jyannkenn_te[0] = 'p';
                MMD_number = 3;
            }
            else {
                DxLib_End();
            }

            //手を出す動画の出力
            WaitTimer(2000);
            play_MMD(MMD_number);


            //じゃんけん結果の処理
            player_te = player_jyannkenn_te[0];
            npc_te = npc_jyannkenn_te[0];

            // win_draw_lose_hantei
            // 1 勝ち、2 引き分け、3 負け（プレイヤーの）
            //デバッグ用、強制勝ち　5
            if (player_te == 'g') {
                if (npc_te == 'g') {
                    win_draw_lose_hantei = 2;
                }
                else if (npc_te == 'c') {
                    win_draw_lose_hantei = 1;
                }
                else if (npc_te == 'p') {
                    win_draw_lose_hantei = 3;
                }
                else {
                    
                }
            }
            else if (player_te == 'c') {
                if (npc_te == 'g') {
                    win_draw_lose_hantei = 3;
                }
                else if (npc_te == 'c') {
                    win_draw_lose_hantei = 2;
                }
                else if (npc_te == 'p') {
                    win_draw_lose_hantei = 1;
                }
                else {
                    
                }
            }
            else if (player_te == 'p') {
                if (npc_te == 'g') {
                    win_draw_lose_hantei = 1;
                }
                else if (npc_te == 'c') {
                    win_draw_lose_hantei = 3;
                }
                else if (npc_te == 'p') {
                    win_draw_lose_hantei = 2;
                }
                else {
            
                }
            }
            else if (player_te == 'w') {
                win_draw_lose_hantei = 5;
            }
            else{
            
            }

            // 入力したじゃんけんの手を表示
            WaitTimer(3000);
            ClearDrawScreen();
            DrawString(0, 0, "あなたの手は", GetColor(255, 255, 255));
            DrawString(0, 16, player_jyannkenn_te, GetColor(255, 255, 255));

            DrawString(0, 30, "コンピュータの手は", GetColor(255, 255, 255));
            DrawString(0, 46, npc_jyannkenn_te, GetColor(255, 255, 255));

            // eを入力したとき(終了用)
            if (re == 2)
            {
                for_kyoka = 0;
            }
            else
            {
                //勝ったら再び行い、負けたら連勝数を示す。
                if (win_draw_lose_hantei == 1) {
                    DrawString(250, 240 - 32, "あなたの勝ち", GetColor(255, 255, 255));
                    rennsyousuu++;
                    for_kyoka = 1;
                }
                else if (win_draw_lose_hantei == 2) {
                    DrawString(250, 240 - 32, "あいこ", GetColor(255, 255, 255));
                    for_kyoka = 1;
                }
                else if (win_draw_lose_hantei == 3) {
                    DrawString(250, 240 - 32, "あなたの負け～", GetColor(255, 255, 255));
                    for_kyoka = 0;
                }
                else if (win_draw_lose_hantei == 5) {
                    DrawString(250, 240 - 32, "あなたの強制勝ち", GetColor(255, 255, 255));
                    rennsyousuu++;
                    for_kyoka = 1;
                }
                else {
                    DxLib_End();
                    return (0);
                }

                DrawFormatString(250, 240, GetColor(255, 255, 255), "連勝数は %d 回 \nエンターキーを押して次に進む", rennsyousuu);

                WaitKey();
                //勝ったらもう一度、負けならタイトル画面に戻る。( Whileループで実装 )
            }
                

                        //配列の初期化
            for (i = 0; i < 10; i++) 
            {
                npc_jyannkenn_te[i] = '\0';
                player_jyannkenn_te[i] = '\0';
            }


        }


        //ランキングの表示

        // 名前、連勝数、時間を「all_score.txt」に記録する
        GetDateTime(&Date);

        Savedata.Day_save = Date.Day;    // 日
        Savedata.Hour_save = Date.Hour;  // 時間
        Savedata.Min_save = Date.Min;    // 分
        Savedata.Sec_save = Date.Sec;    // 秒

        Savedata.rennsyouruu_save = rennsyousuu;

        for (i = 0; i < 10; i++) {
            Savedata.playername_save[i] = username[i];
        }

        FILE* fp_2;
        FILE* fp_1;
        int fp2, fp1;

        fp2 = fopen_s(&fp_2, "all_score.txt", "a");

        if (fp_2 == NULL) {
            //エラーが起きたらNULLを返す
            return 0;
        }

        fprintf(fp_2, "%d,%d,%d,%d,%s,%d\n",
            Savedata.Day_save,
            Savedata.Hour_save,
            Savedata.Min_save,
            Savedata.Sec_save,
            Savedata.playername_save,
            Savedata.rennsyouruu_save);

        // rankingdata

        // ranking.txtに、セーブデータと比較して書き込む
        //ここを、rennsyousuuだけでなく、その他すべての変数を交換する
        for (i = 9; i >= 0; i--) {
            if (rennsyousuu > rankingdata[i].rennsyouruu_ranking) {
                if (i == 9) {
                    rankingdata[i].rennsyouruu_ranking = rennsyousuu;
                    rankingdata[i].Day_ranking = Savedata.Day_save;
                    rankingdata[i].Hour_ranking = Savedata.Hour_save;
                    rankingdata[i].Min_ranking = Savedata.Min_save;
                    rankingdata[i].Sec_ranking = Savedata.Sec_save;

                    for (t = 0; t < 10; t++) {
                        rankingdata[i].playername_ranking[t] = Savedata.playername_save[t];
                    }
                }
                else if (0 <= i && i <= 9) {
                    rankingdata[i + 1] = rankingdata[i];

                    //rankingdata[i].rennsyouruu_ranking = rennsyousuu;
                    rankingdata[i].rennsyouruu_ranking = rennsyousuu;
                    rankingdata[i].Day_ranking = Savedata.Day_save;
                    rankingdata[i].Hour_ranking = Savedata.Hour_save;
                    rankingdata[i].Min_ranking = Savedata.Min_save;
                    rankingdata[i].Sec_ranking = Savedata.Sec_save;

                    for (t = 0; t < 10; t++) {
                        rankingdata[i].playername_ranking[t] = Savedata.playername_save[t];
                    }
                }
            }

        }

        FileRead_close(FileHandle);

        fp1 = fopen_s(&fp_1, "ranking.txt", "w");

        if (ranking_max < 0) {
            ranking_max = 1;
        }

        for (i = 0; i < 10; i++) {
            fprintf(fp_1, "%d,%d,%d,%d,%s,%d\n",
                rankingdata[i].Day_ranking,
                rankingdata[i].Hour_ranking,
                rankingdata[i].Min_ranking,
                rankingdata[i].Sec_ranking,
                rankingdata[i].playername_ranking,
                rankingdata[i].rennsyouruu_ranking);
        }


        // WaitTimer(1000);

        // ・終了前の処理

        // ファイルを閉じる
        FileRead_close(FileHandle);
        fclose(fp_2);  //ファイルを閉じる
        fclose(fp_1);  //ファイルを閉じる

        WaitTimer(300);


    DxLib_End();  // ＤＸライブラリ使用の終了処理
    return 0;     // ソフトの終了
}