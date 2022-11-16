## Shortest path finder

## Compile
In the linux terminal type: ./build.sh <br>
After this to get the list of commands and their uses type: ./alt-dijkstra help<br>

## Test files
Use the following commands to get the node-, egde- and poi files. <br>
wget https://www.idi.ntnu.no/emner/idatt2101/Astjerne/opg/norden/noder.txt <br>
wget https://www.idi.ntnu.no/emner/idatt2101/Astjerne/opg/norden/kanter.txt <br>
wget https://www.idi.ntnu.no/emner/idatt2101/Astjerne/opg/norden/interessepkt.txt <br>

## Implementation of ALT
The ALT algorithm that is used compares the estimated distance from a node to different landmarks. It then chooses the largest estimated distance and uses this in addition to the current travelled distance to decide the priority in the queue. Other than that it is a regular dijkstra algorithm.

### ALT Preprocessing
The preprocessing is done on 2 threads and produces 2 files for each landmark.<br>
The landmarks are in this case predefined, you can change them if need to work better with your map.

## Implementation of Dijkstra for points of interest
The method to get the points of interest around a node uses dijkstra until 8 nodes have been added to a list of indexes. Node type is checked by using a binary and (&) operation on the current nodes type and checks if it is equal to the node type.

## Input Formatting

The program reads txt files for nodes and edges seperately. The format for each is shown below.

### Edge input format
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;16826594         <br>
0&nbsp;1&nbsp;792&nbsp;&nbsp;&nbsp;44&nbsp;&nbsp;&nbsp;20     <br>
1&nbsp;0&nbsp;792&nbsp;&nbsp;&nbsp;44&nbsp;&nbsp;&nbsp;20     <br>
1&nbsp;2&nbsp;1926&nbsp;107&nbsp;20     <br>
2&nbsp;1&nbsp;1926&nbsp;107&nbsp;20     <br>

### Node input format     <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 7509994 <br>
0&nbsp;55.6345298&nbsp;12.0729630 <br>
1&nbsp;55.6345880&nbsp;12.0722614 <br>
2&nbsp;55.6346358&nbsp;12.0705787 <br>
3&nbsp;55.6390613&nbsp;12.0686169 <br>

### Point of interest format <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;264276 <br>
853062&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;&nbsp;&nbsp;"Neste" <br>
2354944&nbsp;1&nbsp;&nbsp;&nbsp;&nbsp;"Myrland" <br>
2856553&nbsp;1&nbsp;&nbsp;&nbsp;&nbsp;"Ulvvik" <br>
6838135&nbsp;1&nbsp;&nbsp;&nbsp;&nbsp;"Salberg" <br>
