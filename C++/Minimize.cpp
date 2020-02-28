#include <iostream>
#include <climits>

using namespace std;
// node, inside any specific node, have a fake pointer points to next node, and another points to merge location
struct node {
	int next_node = 0;
	int merge_to = 0;
};
// navigate shortcut with merged destination
struct navigator {
	int start = 0;
	int dest = 0;
	int disp = 0;
};
// help us to identify which navigate record needs to be handle
struct findLongestHelper {
	int curLongestRec = 0;
	int curLongestDisp = INT_MIN;
};

// parse every node in the graph
void parse_all(node origMap[]);
// differentiate a node is in inside or outside, if it is return 1
int outside_theGraph(node origMap[], int des);
// giving out where a specific will merges to
int merge_toWhere(node origMap[], int init);
// calculate displacement
int calDisp(node origMap[], int init, int des);
// check if the node(beingCheck) is in between of init node and des node, if it is return 1
int inBetween(node origMap[], int init, int des, int beingCheck);
// check if the node(beingCheck) is in between of init node and des node indicate by nav and doneMerge, if it is return 1
int checkMultiple_inBetween(node origMap[], navigator nav[], int mergeCounter, int doneMerge[], int beingCheck);
// find shortcuts in origMap by the new_rule, and store them into nav(with merged destination), return how many shortcuts
int find_shortcuts(node origMap[], int new_rule[], navigator nav[]);
// find out which one is the longest, and put them into helper[], return how many were found
int find_longest(findLongestHelper helper[], navigator nav[], int shortcutCounter);

int main()
{
	int node_counts = 0;
	cin >> node_counts;     // input the numeber of nodes
	// using an array for storing original node rules
	node* nodes = new node[node_counts]();
	node* old_rule = new node[node_counts]();
	for (int i = 0; i < node_counts; i++) {
		cin >> nodes[i].next_node;
	}
	// using another array for storing new rules
	int* new_rule = new int[node_counts]();
	for (int i = 0; i < node_counts; i++)
		cin >> new_rule[i];

	cout << endl;

	// print original, and deal with the easiest
	for (int i = 0; i < node_counts; i++) {
		cout << nodes[i].next_node;
		if (i + 1 != node_counts)
			cout << " ";
		// the last node looks identical to the other easiest nodes, thus, avoid merge that
		if (nodes[i].next_node == -1 && i != (node_counts-1)) {
			nodes[i].next_node = new_rule[i];
			nodes[i].merge_to = new_rule[i];
		}
		old_rule[i].next_node = nodes[i].next_node;
	}
	cout << endl;

	// find shortcuts and store them into nav[]
	navigator* nav = new navigator[100]();
	int shortcutCounter = find_shortcuts(nodes, new_rule, nav);

	// find the longest shortcut and store them into helper[]
	findLongestHelper helper[100] = {};
	int longestCounter = 0;

	int mergeCounter = 1;			// this will record how many shortcuts in nav[] has done 
	int doneMerge[100] = {};		// inside this array will record which shortcut in nav[] has done 
	
	int fuck = 1;
	// the worst case would be 
	while (fuck) {
		if (fuck % 2) {
			// odd round
			// find the longest shortcut and store them into helper[]
			longestCounter = find_longest(helper, nav, shortcutCounter);
			if (longestCounter == 0)
				break;		// longestCounter gets to zero means we done all rule changes

			for (int i = 0; i < node_counts; i++) {
				cout << nodes[i].next_node;
				if (i + 1 != node_counts)
					cout << " ";
			}
			cout << endl;
			doneMerge[mergeCounter - 1] = helper[0].curLongestRec;
			for (int i = 0; i < longestCounter; i++) {
				if (!checkMultiple_inBetween(nodes, nav, mergeCounter, doneMerge, nav[helper[i].curLongestRec].start)) {
					// merge the node and connects to the next one
					nodes[nav[helper[i].curLongestRec].start].next_node = new_rule[nav[helper[i].curLongestRec].start];
					nodes[nav[helper[i].curLongestRec].start].merge_to = new_rule[nav[helper[i].curLongestRec].start];
					if(doneMerge[mergeCounter - 1] != helper[i].curLongestRec){
						mergeCounter++;
						// if we got multiple equal disp. route, since we already put first route inside
						// while merging if we had no this condition it will record first record for twice
						doneMerge[mergeCounter - 1] = helper[i].curLongestRec;
					}
				}
				helper[i].curLongestRec = 0;
				helper[i].curLongestDisp = INT_MIN;
			}

			for (int i = 0; i < node_counts; i++) {
				cout << nodes[i].next_node;
				if (i + 1 != node_counts)
					cout << " ";
			}
			cout << endl;
		}
		else {
			// even round
			for (int i = 0; i < shortcutCounter; i++) {
				if (nav[i].start == -1 || i == doneMerge[i]) {
					continue;
				}
				else if (checkMultiple_inBetween(old_rule, nav, mergeCounter, doneMerge, nav[i].start)) {
					// merge the node and connects to the next one
					nodes[nav[i].start].next_node = new_rule[nav[i].start];
					nodes[nav[i].start].merge_to = new_rule[nav[i].start];
					nav[i].start = -1;		// using -1 to identify this rule has been updeted
				}
			}
			for (int i = 0; i < mergeCounter; i++) {
				nav[doneMerge[i]].start = -1;
				doneMerge[i] = 0;
			}
			mergeCounter = 1;
		}
		fuck++;
	}
	// print original, and deal with the easiest
	for (int i = 0; i < node_counts; i++) {
		if (nodes[i].merge_to == 0)
			cout << "-1";
		else
			cout << nodes[i].next_node;
		nodes[i].merge_to = 0;
		if (i + 1 != node_counts)
			cout << " ";
	}
	cout << endl << "EOF" << endl;
	delete[] nodes;
	delete[] new_rule;
	delete[] nav;

	return 0;
}
// parse every node in the graph
void parse_all(node origMap[]) {
	int curNode = 0;
	while (curNode != -1) {
		cout << curNode << " ";
		curNode = origMap[curNode].next_node;
	}
	cout << endl;
}
// differentiate a node is in inside or outside, if it is return 1
int outside_theGraph(node origMap[], int des){
	int outside = 1;
	int curNode = 0;
	while (curNode != -1) {
		curNode = origMap[curNode].next_node;
		if (curNode == des){	// parsed the des node inside the graph
			outside = 0;
			break;
		}
	}
	return outside;
}
// giving out where a specific will merges to
int merge_toWhere(node origMap[], int init) {
	int curNode = init;
	while (origMap[curNode].merge_to != 0)
		curNode = origMap[curNode].merge_to;	// if it is not 0, go through the next one

	return curNode;								//	if it is 0, means it merges to nowhere
}
// calculate displacement
int calDisp(node origMap[], int init,int des){
	int disp = 0;
	int curNode = init;
	while (curNode != -1) {
		disp++;
		if (origMap[curNode].next_node == des)
			break;
		else
			curNode = origMap[curNode].next_node;
	}
	if (curNode == -1)
		return -1;		// if we reach the -1, means the node is not in the forward direction of graph
	return disp;
}
// check if the node(beingCheck) is in between of init node and des node, if it is return 1
int inBetween(node origMap[], int init, int des, int beingCheck) {
	int curNode = init;
	while (curNode != des && curNode != -1) {
		if (origMap[curNode].next_node == beingCheck && origMap[curNode].next_node != des)	// in order not to count right boundary as between
			return 1;
		else
			curNode = origMap[curNode].next_node;
	}
	return 0;
}
// check if the node(beingCheck) is in between of init node and des node indicate by nav and doneMerge, if it is return 1
int checkMultiple_inBetween(node origMap[], navigator nav[], int mergeCounter, int doneMerge[], int beingCheck) {
	for (int i = 0; i < mergeCounter; i++) {
		if (inBetween(origMap, nav[doneMerge[i]].start, nav[doneMerge[i]].dest, beingCheck)) {
			return true;
		}
	}
	return false;
}
// find shortcuts in origMap by the new_rule, and store them into nav(with merged destination), return how many shortcuts
int find_shortcuts(node origMap[], int new_rule[], navigator nav[]) {
	int shortcutCounter = 0;
	int startNode = 0;
	int destNode = new_rule[startNode];
	while (destNode != -1) {
		// check if the node is outside the graph, if it is outside the graph, we see where it merges to
		if (outside_theGraph(origMap, destNode))
			destNode = merge_toWhere(origMap, destNode);

		nav[shortcutCounter].start = startNode;
		nav[shortcutCounter].dest = destNode;

		nav[shortcutCounter].disp = calDisp(origMap, startNode, destNode);
		// if -1 means we can not see the node in forward direction
		if (nav[shortcutCounter].disp == -1)
			nav[shortcutCounter].disp = -calDisp(origMap, destNode, startNode);

		// go through the next one
		startNode = destNode;
		destNode = new_rule[destNode];

		shortcutCounter++;
	}
	return shortcutCounter;
}
// find out which one is the longest, and put them into helper[], return how many were found
int find_longest(findLongestHelper helper[], navigator nav[], int shortcutCounter) {
	int longestCounter = 0;
	int longestCounterLB = 0;	// left bigger
	// find the longest shortcut
	for (int i = 0; i < shortcutCounter; i++) {
		//cout << nav[i].start << "\t" << nav[i].dest << "\t" << nav[i].disp << endl;
		// after being merged the nav's element will get a start with -1
		if (nav[i].start != -1) {
			if (nav[i].disp > helper[longestCounter].curLongestDisp) {
				helper[longestCounter].curLongestRec = i;
				helper[longestCounter].curLongestDisp = nav[i].disp;
				if(longestCounterLB == 0)
					longestCounterLB = 1;	// separate LB from lonegestCounter, in order to make it can always point to the biggest
			}
			else if (nav[i].disp == helper[longestCounter].curLongestDisp) {
				longestCounter++;
				helper[longestCounter].curLongestRec = i;
				helper[longestCounter].curLongestDisp = nav[i].disp;
			}
		}
	}
	return longestCounter + longestCounterLB;
}
