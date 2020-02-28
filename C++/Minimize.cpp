#include <iostream>
#include <climits>

using namespace std;
// node, inside any specific node, have a fake pointer points to next node, and another points to merge location
struct node {
	int next_node = 0;
	int merge_to = 0;
	int startORend = 0;
};
// navigate shortcut with merged destination
struct navigator {
	int start = 0;
	int dest = 0;
	int disp = 0;
	int proceeded = 0;
};

// differentiate a node is in inside or outside, if it is return 1
int outside_theGraph(node origMap[], int des);
// giving out where a specific will merges to
int merge_toWhere(node origMap[], int init);
// calculate displacement
int calDisp(node origMap[], int init, int des);
// check if the node(beingCheck) is in between of init node and des node, if it is return 1
int inBetween(node origMap[], int init, int des, int beingCheck);
// check if the node(beingCheck) is in between of init node and des node indicate by nav and doneMerge, if it is return 1
int checkMultiple_inBetween(node origMap[], navigator nav[], int shortcutCounter, int beingCheck);
// find shortcuts in origMap by the new_rule, and store them into nav(with merged destination), return how many shortcuts
int find_shortcuts(node origMap[], int new_rule[], navigator nav[]);
// find out which one is the longest, and put them into helper[], return how many were found
int find_longest(int &curLongestRec, navigator nav[], int shortcutCounter);

int main()
{
	int node_counts = 0;
	cin >> node_counts;     // input the numeber of nodes
	// using an array for storing original node rules
	node* nodes = new node[node_counts]();
	node* old_rule = new node[node_counts]();
	int* fLine = new int[node_counts]();
	for (int i = 0; i < node_counts; i++) {
		cin >> nodes[i].next_node;
		fLine[i] = nodes[i].next_node;
	}
	// using another array for storing new rules
	int* new_rule = new int[node_counts]();
	for (int i = 0; i < node_counts; i++)
		cin >> new_rule[i];

	// deal with the easiest
	int easiest_exist = 0;
	for (int i = 0; i < node_counts; i++) {
		// the last node looks identical to the other easiest nodes, thus, avoid merge that
		if (nodes[i].next_node == -1 && i != (node_counts - 1)) {
			easiest_exist = easiest_exist == 0 ? (nodes[i].next_node != new_rule[i] ?1:0) : easiest_exist;
			nodes[i].next_node = new_rule[i];
			nodes[i].merge_to = new_rule[i];
		}
	}

	// find shortcuts and store them into nav[]
	navigator nav[100] = {};
	int shortcutCounter = find_shortcuts(nodes, new_rule, nav);

	// a temperary storage for outputs
	int** Lines = new int*[shortcutCounter * 2]();
	Lines[0] = fLine;
	int lineCounter = 1;
	for (int i = lineCounter; i < shortcutCounter * 2; i++)
		Lines[i] = new int[node_counts]();

	// find the longest shortcut and store them into helper[]
	int curLongestRec = 0;
	int longestCounter = 0;

	int roundIndicator = 1;
	while (roundIndicator) {
		if (roundIndicator % 2) {
			// odd round
			for (int i = 0; i < node_counts; i++) {
				old_rule[i].next_node = nodes[i].next_node;
				old_rule[i].merge_to = nodes[i].merge_to;
				old_rule[i].startORend = nodes[i].startORend;
			}
			// find the longest shortcut and store them into helper[]
			longestCounter = find_longest(curLongestRec, nav, shortcutCounter);
			if (longestCounter == 0)
				break;		// longestCounter gets to zero means we done all rule changes

			for (int i = 0; i < node_counts; i++) {
				Lines[lineCounter][i] = nodes[i].next_node;		// record the result in here
			}
			lineCounter = easiest_exist == 1 ? lineCounter + 1: lineCounter;
			easiest_exist = 1;

			nodes[nav[curLongestRec].start].next_node = new_rule[nav[curLongestRec].start];
			nodes[nav[curLongestRec].start].merge_to = new_rule[nav[curLongestRec].start];
			nodes[nav[curLongestRec].start].startORend--;
			nav[curLongestRec].proceeded = 1;

			// if we can have multiple route done, done others here
			for (int i = 0; i < shortcutCounter; i++) {
				if (i == curLongestRec || nav[i].proceeded == -1 || nav[i].disp < 0)
					continue;

				if (!checkMultiple_inBetween(old_rule, nav, shortcutCounter, nav[i].start) && !checkMultiple_inBetween(old_rule, nav, shortcutCounter, merge_toWhere(old_rule, nav[i].dest))) {
					// merge the node and connects to the next one
					nodes[nav[i].start].next_node = new_rule[nav[i].start];
					nodes[nav[i].start].merge_to = new_rule[nav[i].start];
					nodes[nav[i].start].startORend--;
					nav[i].proceeded = 1;
				}
			}

			curLongestRec = 0;

			for (int i = 0; i < node_counts; i++) {
				Lines[lineCounter][i] = nodes[i].next_node;		// record the result in here
			}
			lineCounter++;
		}
		else {
			// even round
			for (int i = 0; i < shortcutCounter; i++) {
				if (nav[i].proceeded == 0 && checkMultiple_inBetween(old_rule, nav, shortcutCounter, nav[i].start)) {
					// merge the node and connects to the next one
					nodes[nav[i].start].next_node = new_rule[nav[i].start];
					nodes[nav[i].start].merge_to = new_rule[nav[i].start];
					nodes[nav[i].start].startORend--;
					nav[i].proceeded = 1;
				}
			}

			for (int i = 0; i < shortcutCounter; i++) {
				if (nav[i].proceeded == 1)
					nav[i].proceeded = -1;
				else {
					if (outside_theGraph(nodes, nav[i].dest)){
						nav[i].dest = merge_toWhere(nodes, nav[i].dest);
					}
					nav[i].disp = calDisp(nodes, nav[i].start, nav[i].dest);
					if (nav[i].disp == 0)
						nav[i].disp = -calDisp(nodes, nav[i].dest, nav[i].start);
				}
			}
		}
		roundIndicator++;
	}
	easiest_exist = 0;
	// to deal with outside nodes, and clean merge_to
	for (int i = 0; i < node_counts; i++) {
		if (nodes[i].merge_to == 0 && i != (node_counts - 1)) {
			easiest_exist = easiest_exist == 0 ? 1 : easiest_exist;
			Lines[lineCounter][i] = -1;
		}
		else {
			Lines[lineCounter][i] = nodes[i].next_node;
		}
		nodes[i].merge_to = 0;
	}
	lineCounter = easiest_exist == 1 ? lineCounter + 1 : lineCounter;

	cout << lineCounter << endl;
	for (int i = 0; i < lineCounter; i++) {
		for (int j = 0; j < node_counts; j++) {
			cout << Lines[i][j];
			if (j + 1 != node_counts)
				cout << " ";
		}
		cout << endl;
	}
	
	delete[] nodes;
	delete[] new_rule;
	for (int i = lineCounter; i < shortcutCounter * 2; i++)
		delete[] Lines[i];
	delete[] Lines;

	return 0;
}

// differentiate a node is in inside or outside, if it is return 1
int outside_theGraph(node origMap[], int des) {
	int outside = 1;
	int curNode = 0;
	while (curNode != -1) {
		curNode = origMap[curNode].next_node;
		if (curNode == des) {	// parsed the des node inside the graph
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
int calDisp(node origMap[], int init, int des) {
	int disp = 1;
	int curNode = init;
	while (curNode != -1) {
		if (origMap[curNode].next_node == des)
			break;

		if (origMap[origMap[curNode].next_node].startORend)
			disp++;

		curNode = origMap[curNode].next_node;
	}
	if (curNode == -1)
		return 0;		// if we reach the -1, means the node is not in the forward direction of graph
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
int checkMultiple_inBetween(node origMap[], navigator nav[], int shortcutCounter, int beingCheck) {
	for (int i = 0; i < shortcutCounter; i++) {
		if (nav[i].proceeded == 1) {
			if (nav[i].disp >= 0) {
				if(inBetween(origMap, nav[i].start, nav[i].dest, beingCheck))
					return 1;
			}				
			else {
				if (inBetween(origMap, nav[i].dest, nav[i].start, beingCheck))
					return 1;
			}
		}
	}
	return 0;
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
		if (origMap[startNode].next_node != new_rule[startNode]) {
			nav[shortcutCounter].start = startNode;
			nav[shortcutCounter].dest = destNode;
			origMap[startNode].startORend++;
			shortcutCounter++;
		}
		else {
			origMap[startNode].merge_to = origMap[startNode].next_node;
		}
		// go through the next one
		startNode = destNode;
		destNode = new_rule[destNode];
	}
	for (int i = 0; i < shortcutCounter; i++) {
		nav[i].disp = calDisp(origMap, nav[i].start, nav[i].dest);
		// if 0 means we can not see the node in forward direction
		if (nav[i].disp == 0)	// check the reverse direction
			nav[i].disp = -calDisp(origMap, nav[i].dest, nav[i].start);
	}
	return shortcutCounter;
}
// find out which one is the longest, and put them into helper[], return how many were found
int find_longest(int &curLongestRec, navigator nav[], int shortcutCounter) {
	int longestCounter = 0;
	int curLongestDisp = INT_MIN;
	// find the longest shortcut
	for (int i = 0; i < shortcutCounter; i++) {
		// after being proceeded the nav's proceeded element will be -1
		if (nav[i].proceeded == 0) {
			if (nav[i].disp > curLongestDisp) {
				curLongestRec = i;
				curLongestDisp = nav[i].disp;
				if (longestCounter == 0)
					longestCounter = 1;
			}
			else if (nav[i].disp == curLongestDisp && nav[i].start < nav[curLongestRec].start) {
				curLongestRec = i;
				curLongestDisp = nav[i].disp;
				if (longestCounter == 0)
					longestCounter = 1;
			}
		}
	}
	return longestCounter;
}