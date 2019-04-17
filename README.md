BTTE (BitTorrent Testing Environment)
====


## 簡介
- 這是一個 BitTorrent (BT) 的模擬測試環境，對 BT 有興趣的人可以運行此環境了解其中運行的細節
- 可以用此環境運行客製化 BT 的演算法，並與原始的通訊協定作比較
- 此環境實作了最原始的 BitTorrent Protocol：
  - Tracker, Peer
  - Piece (packet)
  - Event Handler
  - Peer Selection, Piece Selection, Choking
- 本環境也實作了兩個客製化的 Peer Selection 演算法：
  - Cluster Based
  - Load Balancing
- 本專案是在 single thread 和 local machine 上運行，所以利用「亂數」來模擬網路環境
  
## 主要模組
<p align="center">
  <img src="/images/modules.png?raw=true">
</p>

- EnvManager(`env_manager.cpp`): 管理整個模擬程式的創建和銷毀
- EventHandler(`event_handler.cpp`): 掌管事件的產生和結束，並且確保事件之間的依賴性是正確的（某些事件有前後順序之分）
- PeerManager (`peer_manager.cpp`): 創造 BT 環境和 Peer 資料
- Algorithm（包含 BT 通訊協定的演算法）(`algorithm/{*.h, *.cpp}`)
  - PeerSelection
  - PieceSelection
  - Choking
- Random: 針對環境中不同的元素產生亂數種子，來模擬一個網路環境。這些元素包含：
  - 不同種 Peer 的 propagation delay
  - 事件的 Timestamp, 例如: peer 的加入和離開時間
  - 亂數產生的 Peer List
  - 演算法內所需要的亂數
- IO: 讀取設定檔中的參數及輸出演算法運行結果

## 輸入
設定檔 `/btte.conf` 包含以下幾種參數：
- Peer:
  - NUM_PEER
  - NUM_SEED: 預設已經擁有全部資源的 peers 數量
  - NUM_LEECH: 必須執行下載任務的 peers 數量
  - NUM_PIECE: Peer 總共能下載的資源數量
  - ARRIVAL_RATE: Peer 加入的頻率
- Tracker:
  - NUM_PEERLIST: Peers 列表中的節點數量
- Algorithm:
  - NUM_CHOKING: 此數值決定 peer 最多能與幾個其他的 peers 分享
  - TYPE_PEERSELECT: 決定此環境運行哪一種 Peer Selection
  - TYPE_PIECESELECT: 決定此環境運行哪一種 Piece Selection
  
## 輸出
- Log file，紀錄當中包含四種屬性:
  - Peer ID
  - Join Time
  - Finish Time: 取得所有資源（piece）的時間點
  - Departure Time: 離開網路的時間點

- Result Chart:
  - 此圖呈現不同演算法的平均下載時間
  - 橫軸：必須下載的資源數量 (piece) 
  - 縱軸：完成下載所消耗的平均時間（average complete time)
  
<p align="center">
  <img src="/images/result_chart.png?raw=true">
</p>
  
  
