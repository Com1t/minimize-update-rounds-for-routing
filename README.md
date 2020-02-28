# minimize-update-rounds-for-routing
Using Round-Based Update Algorithm to minimize update rounds routing path update in SDN
# Update in SDN
Sach switch receives the update message and updates its rule independently and asynchronously.
![image](https://github.com/Com1t/minimize-update-rounds-for-routing/blob/master/DEMO/001.png)
To avoid update breaks the connectivity.
# Round-Based Update Algorithm
Use Round-Based Update Algorithm can keep the connectivity
Updates the longest in odd round
![image](https://github.com/Com1t/minimize-update-rounds-for-routing/blob/master/DEMO/002.png)
Updates those won't affect to the connectivity in even round
![image](https://github.com/Com1t/minimize-update-rounds-for-routing/blob/master/DEMO/003.png)

# Ref
CCU data structure
