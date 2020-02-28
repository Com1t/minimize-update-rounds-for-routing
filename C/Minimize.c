#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// node, inside any specific node, have a fake pointer points to next node, and another points to merge location
typedef struct {
	int next_node, merge_to;
} node;
// navigate shortcut with merged destination
typedef struct {
	int start, dest, disp;
} navigator;
// help us to identify which navigate record needs to be handle
typedef struct {
	int curLongestRec;
	int curLongestDisp;
} findLongestHelper;

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
int find_longest(findLongestHelper *helper, navigator nav[], int shortcutCounter);

int main()
{
	int node_counts = 0;
	scanf("%d", &node_counts);     // input the numeber of nodes
	// using an array for storing original node rules
	node* nodes = malloc(sizeof(node)*node_counts);
	
	int* fLine = calloc(node_counts, sizeof(int));
	for (int i = 0; i < node_counts; i++) {
		scanf("%d", &nodes[i].next_node);
		fLine[i] = nodes[i].next_node;
		nodes[i].merge_to = 0;
	}

	// using another array for storing new rules
	node* new_nodes = malloc(sizeof(node)*node_counts);
	int* new_rule = calloc(node_counts, sizeof(int));
	for (int i = 0; i < node_counts; i++){
		scanf("%d", &new_rule[i]);
		new_nodes[i].next_node = new_rule[i];
		new_nodes[i].merge_to = 0;
	}
	
	node* old_rule = malloc(sizeof(node)*node_counts);
	for (int i = 0; i < node_counts; i++) {	// initiation
		old_rule[i].next_node = 0;
		old_rule[i].merge_to = 0;
	}
	// deal with the easiest
	for (int i = 0; i < node_counts; i++) {
		// the last node looks identical to the other easiest nodes, thus, avoid merge that
		if (nodes[i].next_node == -1 && i != (node_counts - 1)) {
			nodes[i].next_node = new_rule[i];
			nodes[i].merge_to = new_rule[i];
		}
		old_rule[i].next_node = nodes[i].next_node;
	}

	// find shortcuts and store them into nav[]
	navigator nav[100] = {0, 0, 0};
	int shortcutCounter = find_shortcuts(nodes, new_rule, nav);
	
	// a temperary storage for outputs
	int** Lines = calloc(shortcutCounter*2, sizeof(int*));
	Lines[0] = fLine;
	int lineCounter = 1;
	for (int i = lineCounter; i < shortcutCounter * 2; i++)
		Lines[i] = calloc(node_counts, sizeof(int));

	// find the longest shortcut and store them into helper[]
	findLongestHelper helper = {0, INT_MIN};
	int longestCounter = 0;

	int mergeCounter = 1;			// this will record how many shortcuts in nav[] has done 
	int doneMerge[100] = {};		// inside this array will record which shortcut in nav[] has done 

	int roundIndicator = 1;
	while (roundIndicator) {
		if (roundIndicator % 2) {
			// odd round
			// find the longest shortcut and store them into helper[]
			longestCounter = find_longest(&helper, nav, shortcutCounter);
			if (longestCounter == 0)
				break;		// longestCounter gets to zero means we done all rule changes

			for (int i = 0; i < node_counts; i++) {
				Lines[lineCounter][i] = nodes[i].next_node;
			}
			lineCounter++;

			doneMerge[mergeCounter - 1] = helper.curLongestRec;

			nodes[nav[helper.curLongestRec].start].next_node = new_rule[nav[helper.curLongestRec].start];
			nodes[nav[helper.curLongestRec].start].merge_to = new_rule[nav[helper.curLongestRec].start];

			helper.curLongestRec = 0;
			helper.curLongestDisp = INT_MIN;
			
			// if we can have multiple route done, done others here
			for (int i = 0; i < shortcutCounter; i++) {
				if (nav[i].start == -1 || i == doneMerge[i]) {
					continue;
				}
				else if (!checkMultiple_inBetween(old_rule, nav, mergeCounter, doneMerge, nav[i].start) && !checkMultiple_inBetween(old_rule, nav, mergeCounter, doneMerge, merge_toWhere(nodes, nav[i].dest))) {
					// merge the node and connects to the next one
					nodes[nav[i].start].next_node = new_rule[nav[i].start];
					nodes[nav[i].start].merge_to = new_rule[nav[i].start];
					mergeCounter++;
					doneMerge[mergeCounter - 1] = i;
				}
			}
			printf("Round %d\n", roundIndicator);
			for(int i = 0; i < shortcutCounter; i ++)
				printf("%d %d %d\n", nav[i].start, merge_toWhere(nodes, nav[i].dest), nav[i].disp);
			printf("\n");
			for (int i = 0; i < node_counts; i++) {
				Lines[lineCounter][i] = nodes[i].next_node;
			}
			lineCounter++;
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
		roundIndicator++;
	}
	// to deal with outside nodes, and clean merge_to
	for (int i = 0; i < node_counts; i++) {
		if (nodes[i].merge_to == 0)
			Lines[lineCounter][i] = -1;
		else
			Lines[lineCounter][i] = nodes[i].next_node;
		nodes[i].merge_to = 0;
	}
	lineCounter++;
	
	printf("%d\n", lineCounter);
	for (int i = 0; i < lineCounter; i++){
		for (int j = 0; j < node_counts; j++) {
			printf("%d", Lines[i][j]);
			if (j + 1 != node_counts)
				printf(" ");
		}
		printf("\n");
	}
	
	free(new_rule);
	for (int i = lineCounter; i < shortcutCounter * 2; i++)
		free(Lines[i]);
	free(Lines);

	return 0;
}
// parse every node in the graph
void parse_all(node origMap[]) {
	int curNode = 0;
	while (curNode != -1) {
		printf("%d ", curNode);
		curNode = origMap[curNode].next_node;
	}
	printf("\n");
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
			return 1;
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
int find_longest(findLongestHelper *helper, navigator nav[], int shortcutCounter){
	int longestCounter = 0;
	// find the longest shortcut
	for (int i = 0; i < shortcutCounter; i++) {
		// after being merged the nav's element will get a start with -1
		if (nav[i].start != -1) {
			if (nav[i].disp > helper->curLongestDisp) {
				helper->curLongestRec = i;
				helper->curLongestDisp = nav[i].disp;
				if(longestCounter == 0)
					longestCounter = 1;
			}
			else if (nav[i].disp == helper->curLongestDisp && nav[i].start < nav[helper->curLongestRec].start) {
				helper->curLongestRec = i;
				helper->curLongestDisp = nav[i].disp;
				if (longestCounter == 0)
					longestCounter = 1;
			}
		}
	}
	return longestCounter;
}
