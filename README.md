BTTE (BitTorrent Testing Environment)
====


## 簡介
- 這是一個 BitTorrent (BT) 的模擬測試環境，對 BT 有興趣的人可以運行此環境了解其中運行的細節
- 可以用此環境運行客製化 BT 的演算法，並與原始的通訊協定作比較
- 此環境實作了[最原始的 BitTorrent Protocol](http://bittorrent.org/bittorrentecon.pdf)：
  - Tracker, Peer
  - Piece (packet)
  - Event Handler
  - Peer Selection, Piece Selection, Choking
- 本環境也實作了兩個客製化的 Peer Selection 演算法：
  - Cluster Based
  - Load Balancing
- 本專案是在 single thread 和 local machine 上運行，所以利用「亂數」來模擬網路環境
- 本專案的文件[連結](https://drive.google.com/open?id=1fwoZJP2ln-4Wf4zJ53H3-MDaFTxFFw_j)


## 安裝及使用
### 前置需求:
  - g++
  - cmake
  - python3.x
  - gnuplot (for result chart)
  - git (optional)
  
### 安裝:
  1. `git clone https://github.com/ckybonist/BTTE`
  
  2. `./bootstrap.sh`
  
  3. `./build.sh [options]`, option includes:
  - debug: Debug build
  - release: Release build
  - makeall: Build both debug and release
  - rebuild: Remove and create new `obj/` directory, then execute `makeall` 
    (it's necessary when you add new files in `src/`) 
      
      
### 使用:
  1. Enter to program path: `cd [obj/release/bin | obj/debug/bin]`
  
  2. Run: `./BTTE btte.conf [peer_sel | piece_sel]`
  - `btte.conf`: Parameters of the testing environment
  - `peer_sel and piece_sel`: 
  This option tells the program that which types of algorithm (Peer Selection or Piece Selection) results should be collected. E.g. If peer_sel is given, then the output filenames will be the three Peer Selection algorithms (i.e. Standard.log, LoadBalacing.log and ClusterBased.log) respectivly.
    
  3. Generate result chart: `./script/plot/plot.sh [plot_type] [algo_type]`
  - plot_type: time_peer or time_piece (time vs. peer or time vs. piece)
  - algo_type: peer_sel, piece_sel
  - chart path: `plot/output/*.png`
    
    
  4. Implment your custom algorithms:
  - `cd custom/`
  -  For peer selection: edit `my_peer_selection.cpp`
  - For piece selection: edit `my_piece_selection.cpp`
  - All newly added files should be placed in `custom/`
  - Rebuild: `./script/build.sh rebuild`


  
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
  
  
