#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
//#include <math.h>

enum
{
	Add = 1,
	Sub = 2,
	Mul = 3,
	Div = 4,
	var = 5,
	operation = 6,
	func = 7,
	num = 8,
	varx = 9,
	power = 10,
};

enum functions
{
	sin = 15,
	cos = 16,
};

struct NODE
{
	int data;
	NODE* left;
	NODE* right;
	NODE* prev;
	int call;
	int type;
};

const char* s = "";

int g_number = 0;

NODE* GetG(char*);

NODE* GetE();

NODE* GetT();

NODE* GetP();

NODE* GetN();

NODE* GetPow();

NODE* Create_node(int, int);

int Draw(NODE*);

int Draw_param(NODE*, FILE*);

int Draw_tree(NODE*, FILE*, int, char*);

int Graphviz_prepare(NODE*, int);

char* operations();

NODE* Differenciator(NODE*);

int PrintTree(NODE*, FILE*, char*);

int PrintNode(NODE*, FILE*, char*);

int priory(NODE*);

NODE* Copy(NODE*);

int Optimizator(NODE*);

int add_sub_optimize(NODE*);

int mul_optimize(NODE*);

int power_optimize(NODE*);

int second_link(NODE*, NODE*);

int main()
{
	FILE* input = fopen("input.txt", "r");
	fseek(input, 0, SEEK_END);
	int size = ftell(input);
	fseek(input, 0, SEEK_SET);

	char* str = (char*)calloc(size, sizeof(char));

	fscanf(input, "%s", str);

	NODE* head = GetG(str);

	NODE* diff = Differenciator(head);

	//second_link(diff, 0);

	Optimizator(diff);
	Optimizator(diff);
	Optimizator(diff);

	Draw(diff);

	char* commands = operations();

	FILE* output = fopen("output.txt", "w");

	PrintTree(diff, output, commands);

	system("dot -Tpng D:\\vs_projects\\Calculator\\graph_code_test.txt -oD:\\vs_projects\\Calculator\\graph_image.png");

	return 0;
}

char* operations()
{
	char* operations = (char*)calloc(20, sizeof(char));

	operations[Add] = '+';
	operations[Sub] = '-';
	operations[Mul] = '*';
	operations[Div] = '/';
	operations[power] = '^';
	operations[varx] = 'x';

	return operations;
}

NODE* GetG(char* str)
{
	s = str;
	NODE* head = GetE();
	assert(*s == '\0');

	return head;
}

NODE* GetE()
{
	NODE* left_node = GetT();

	while (*s == '+' || *s == '-')
	{
		char op = *s;
		s++;

		NODE* right_node = GetT();

		NODE* new_node = 0;

		if (op == '+')
			new_node = Create_node(Add, operation);
		else
			new_node = Create_node(Sub, operation);

		new_node->left = left_node;

		new_node->right = right_node;

		left_node = new_node;
	}

	return left_node;
}

NODE* GetT()
{
	NODE* left_node = GetPow();

	while (*s == '*' || *s == '/')
	{
		char op = *s;
		s++;

		NODE* right_node = GetPow();

		NODE* new_node = 0;

		if (op == '*')
			new_node = Create_node(Mul, operation);
		else
			new_node = Create_node(Div, operation);

		new_node->left = left_node;
		new_node->right = right_node;

		left_node = new_node;
	}

	return left_node;
}

NODE* GetPow()
{
	NODE* left_node = GetP();

	if (*s == '^')
	{
		char op = *s;
		s++;

		NODE* right_node = GetP();

		NODE* new_node = Create_node(power, operation);

		new_node->left = left_node;
		new_node->right = right_node;

		left_node = new_node;
	}

	return left_node;
}

NODE* GetP()
{
	if (*s == '(')
	{
		s++;
		NODE* inside = GetE();
		assert(*s == ')');
		s++;
		return inside;
	}

	else if (*s == 'x')
	{
		s++;
		return Create_node(varx, var);
	}

	else
		return GetN();
}

NODE* GetN()
{
	int val = 0;

	while ('0' <= *s && *s <= '9')
	{
		val = 10 * val + (*s - '0');
		s++;
	}

	return Create_node(val, num);
}

NODE* Create_node(int data, int type)
{
	NODE* new_node = (NODE*)calloc(1, sizeof(NODE));

	new_node->data = data;

	new_node->type = type;

	new_node->call = g_number;

	g_number++;

	return new_node;
}

int second_link(NODE* node, NODE* prev_node)
{
	node->prev = prev_node;

	if (node->left)
		second_link(node->left, node);

	if (node->right)
		second_link(node->right, node);

	return 0;
}

int Draw(NODE* node)
{
	FILE* output = fopen("graph_code_test.txt", "w");

	char* commands = operations();
	
	Draw_param(node, output);
	Draw_tree(node, output, 1, commands);
	fprintf(output, "}\n}");

	fclose(output);

	return 0;
}

int Draw_param(NODE* node, FILE* output)
{
	fprintf(output, "digraph MyPlan {\n");
	fprintf(output, "  node[shape = \"circle\", style = \"filled\", fillcolor = \"white\", fontcolor = \"#FFFFFF\", margin = \"0.01\"];\n");
	fprintf(output, "  edge [style=\"dashed\"];\n");

	fprintf(output, "{\n node[shape = \"plaintext\",style = \"invisible\"];\n edge[color = \"white\"];\n 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10;\n}\n");

	fprintf(output, "subgraph tree {\n   node [shape=\"ellipse\", style=\"filled\", fillcolor=\"white\", fontcolor=\"black\", fontsize=\"15\"];\n");

	return 0;
}

int Draw_tree(NODE* node, FILE* output, int level, char* commands)
{
	if (node->left)
	{
		if (node->type == num)
		{
			if (node->left->type == num)
			{
				fprintf(output, "   { rank = \"same\"; \"%d\"; \"%d type = %p\"; }\n", level, node->data, &(node->data));
				fprintf(output, "   \"%d type = %p\"->\"%d type = %p\";\n", node->data, &(node->data), node->left->data, &(node->left->data));
			}

			else
			{
				fprintf(output, "   { rank = \"same\"; \"%d\"; \"%d type = %p\"; }\n", level, node->data, &(node->data));
				fprintf(output, "   \"%d type = %p\"->\"%c type = %p\";\n", node->data, &(node->data), commands[node->left->data], &(node->left->data));
			}
		}

		else if (node->left->type == num)
		{
			fprintf(output, "   { rank = \"same\"; \"%d\"; \"%c type = %p\"; }\n", level, commands[node->data], &(node->data));
			fprintf(output, "   \"%c type = %p\"->\"%d type = %p\";\n", commands[node->data], &(node->data), node->left->data, &(node->left->data));
		}

		else
		{
			fprintf(output, "   { rank = \"same\"; \"%d\"; \"%c type = %p\"; }\n", level, commands[node->data], &(node->data));
			fprintf(output, "   \"%c type = %p\"->\"%c type = %p\";\n", commands[node->data], &(node->data), commands[node->left->data], &(node->left->data));
		}

		Draw_tree(node->left, output, (level + 1), commands);
	}


	if (node->right)
	{
		if (node->type == num)
		{
			if (node->right->type == num)
			{
				fprintf(output, "   { rank = \"same\"; \"%d\"; \"%d type = %p\"; }\n", level, node->data, &(node->data));
				fprintf(output, "   \"%d type = %p\"->\"%d type = %p\";\n", node->data, &(node->data), node->right->data, &(node->right->data));
			}

			else
			{
				fprintf(output, "   { rank = \"same\"; \"%d\"; \"%d type = %p\"; }\n", level, node->data, &(node->data));
				fprintf(output, "   \"%d type = %p\"->\"%c type = %p\";\n", node->data, &(node->data), commands[node->right->data], &(node->right->data));
			}
		}

		else if (node->right->type == num)
		{
			fprintf(output, "   { rank = \"same\"; \"%d\"; \"%c type = %p\"; }\n", level, commands[node->data], &(node->data));
			fprintf(output, "   \"%c type = %p\"->\"%d type = %p\";\n", commands[node->data], &(node->data), node->right->data, &(node->right->data));
		}

		else
		{
			fprintf(output, "   { rank = \"same\"; \"%d\"; \"%c type = %p\"; }\n", level, commands[node->data], &(node->data));
			fprintf(output, "   \"%c type = %p\"->\"%c type = %p\";\n", commands[node->data], &(node->data), commands[node->right->data], &(node->right->data));
		}

		Draw_tree(node->right, output, (level + 1), commands);
	}

	return 0;
}

int Graphviz_prepare(NODE* node, int number)
{
	node->call = number;

	if (node->left)
		Graphviz_prepare(node->left, number + 1);

	if (node->right)
		Graphviz_prepare(node->right, number + 1);

	return 0;
}

NODE* Differenciator(NODE* node)
{
	if (node->type == var)
		return Create_node(1, num);
	
	if (node->type == num)
		return Create_node(0, num);

	if (node->type == operation)
	{
		if (node->data == Add) {
			NODE* high_node = Create_node(Add, operation);

			NODE* left_node = Differenciator(node->left);
			NODE* right_node = Differenciator(node->right);

			high_node->left = left_node;
			high_node->right = right_node;

			return high_node;
		}

		if (node->data == Sub) {
			NODE* high_node = Create_node(Sub, operation);

			NODE* left_node = Differenciator(node->left);
			NODE* right_node = Differenciator(node->right);

			high_node->left = left_node;
			high_node->right = right_node;

			return high_node;
		}

		if (node->data == Mul) {
			NODE* high_node = Create_node(Add, operation);

			NODE* left_mul = Create_node(Mul, operation);
			NODE* right_mul = Create_node(Mul, operation);

			left_mul->left = Differenciator(node->left);
			left_mul->right = Copy(node->right);

			right_mul->left = Copy(node->left);
			right_mul->right = Differenciator(node->right);

			high_node->left = left_mul;
			high_node->right = right_mul;

			return high_node;
		}

		if (node->data == power) {

			if (node->right->type == num) {

				NODE* mul_1 = Create_node(Mul, operation);

				NODE* new_mul = Create_node(node->right->data, num);

				mul_1->right = new_mul;

				NODE* mul_2 = Create_node(Mul, operation);

				mul_1->left = mul_2;

				mul_2->left = Differenciator(node->left);

				mul_2->right = Create_node(power, operation);

				mul_2->right->right = Create_node(node->right->data - 1, num);

				mul_2->right->left = Copy(node->left);

				return mul_1;
			}
		}
	}
}

int PrintTree(NODE* node, FILE* output, char* operations)
{
	if (priory(node) >= 10)
		fprintf(output, "(");

	if (node->left)
		PrintTree(node->left, output, operations);

	if (priory(node) >= 10)
		fprintf(output, ")");

	PrintNode(node, output, operations);

	if (priory(node) % 2 == 1)
		fprintf(output, "(");

	if (node->right)
		PrintTree(node->right, output, operations);

	if (priory(node) % 2 == 1)
		fprintf(output, ")");

	return 0;
}

NODE* Copy(NODE* node)
{
	NODE* new_node = Create_node(node->data, node->type);

	if (node->left)
		new_node->left = Copy(node->left);

	if (node->right)
		new_node->right = Copy(node->right);

	return new_node;
}

int PrintNode(NODE* node, FILE* output, char* operations)
{
	if (node->type == num)
		fprintf(output, "%d", node->data);
	else if (node->type == operation)
		fprintf(output, "%c", operations[node->data]);
	else if (node->type == var)
		fprintf(output, "x");

	return 0;
}

int priory(NODE* node)
{
	int res = 0;

	if (node->data == power)
		res = 31;

	if ((node->type != operation) || (node->left->type != operation))
		return res;

	if ((node->data == Mul) || (node->data == Div))
		if ((node->left->data == Add) || (node->left->data == Sub))
			res = 10;

	if ((node->data == Mul) || (node->data == Div))
		if ((node->right->data == Add) || (node->right->data == Sub))
			res++;

	return res;
}

int Optimizator(NODE* node)
{
	if (node->type == operation)
	{
		if (node->data == Mul)
			mul_optimize(node);

		if (node->data == Add || node->data == Sub)
			add_sub_optimize(node);

		if (node->data == power)
			power_optimize(node);
	}

	if (node->left)
		Optimizator(node->left);

	if (node->right)
		Optimizator(node->right);

	return 0;
}

int mul_optimize(NODE* node)
{
	if (node->left == nullptr)
		return 0;

	if ((node->right->data == 0) || (node->left->data == 0))
	{
		node->data = 0;
		node->type = num;
		node->left = nullptr;
		node->right = nullptr;

		return 0;
	}

	if ((node->left->type == num) && (node->right->type == num))
	{
		node->type = num;
		node->data = node->left->data * node->right->data;
		node->left = nullptr;
		node->right = nullptr;

		return 0;
	}

	return 0;
}

int add_sub_optimize(NODE* node)
{
	if (node->left == nullptr)
		return 0;

	if ((node->left->type == num) && (node->right->type == num))
	{
		node->type = num;

		if (node->data == Add)
			node->data = node->left->data + node->right->data;
		else
			node->data = node->left->data - node->right->data;
		
		node->left = nullptr;
		node->right = nullptr;

		return 0;
	}

	if ((node->left->type == num) && (node->left->data == 0) && (node->data == Add))
	{
		NODE* to_delete = node->right;

		node->type = to_delete->type;
		node->data = to_delete->data;
		node->left = to_delete->left;
		node->right = to_delete->right;
		node->call = to_delete->call;
		
		return 0;
	}

	if ((node->right->type == num) && (node->right->data == 0))
	{
		NODE* to_delete = node->left;

		node->type = to_delete->type;
		node->data = to_delete->data;
		node->left = to_delete->left;
		node->right = to_delete->right;
		node->call = to_delete->call;

		if (node->data == Sub)
			printf("2");

		return 0;
	}

	return 0;
}

int power_optimize(NODE* node)
{
	if (node->left == nullptr)
		return 0;

	if (node->right->type == num && node->right->data == 1)
	{
		NODE* to_delete = node->left;

		node->type = to_delete->type;
		node->data = to_delete->data;
		node->left = to_delete->left;
		node->right = to_delete->right;
		node->call = to_delete->call;

		return 0;
	}
		
	return 0;
}