# 工場の気温を管理するプログラム   

## wifiと計測場所の設定    
 /src/main.cpp 8行目付近。
```C++
//-------------------------設定------------------------------//
String locat = "生産技術";//計測場所名
int interval = 30000;//データの更新間隔（ms）
const char* url = "https://hogehoge";//json受け取りサーバー

char * wifi_setup[] = {
  //"SSID","password",
  "SSID","password"
};
//----------------------------------------------------------//
```  
 上記の設定項目を参考に書きを入力してください。  
1. wifiの**SSID**,**PASSWORD**を入力　* カンマで区切れば、複数登録可能   
1. 計測場所名は計測する場所を自由に設定できます。   
`※計測場所名は、自由に名前をつけれますが  
グラフ化するときにどこのデータかがわからなくなることがありますので、わかりやすい名前にしてください。`    
1. 予め要してあるBIツールのURLを入力。

 ## データの書き込み  
 このプログラムは、VScodeのPlatformIOにてコンパイルしてください。  
 シリアル通信のボーレートは、115200bps です。  


