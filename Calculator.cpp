#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
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
	noth = 11,
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

NODE* GetF();

NODE* Create_node(int, int);

int Draw(NODE*);

int Draw_param(NODE*, FILE*);

int Draw_tree(NODE*, FILE*, int, char*);

int Graphviz_prepare(NODE*, int);

char* operations_array();

char** functions_array();

NODE* Differenciator(NODE*);

int PrintTree(NODE*, FILE*, char*, char**);

int PrintNode(NODE*, FILE*, char*, char**);

int DetectFunction();

int priory(NODE*);

int priory_pro(NODE*);

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

	//Draw(diff);
	

	char* commands = operations_array();

	char** functions = functions_array();

	FILE* output = fopen("output.txt", "w");

	PrintTree(diff, output, commands, functions);

	system("dot -Tpng D:\\vs_projects\\Calculator\\graph_code_test.txt -oD:\\vs_projects\\Calculator\\graph_image.png");

	return 0;
}

char* operations_array()
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

char** functions_array()
{
	char** functions = (char**)calloc(20, sizeof(char*));

	for (int i = 0; i < 20; i++)
	{
		functions[i] = (char*)calloc(10, sizeof(char));
	}
    
	char fsin[10] = "sin";
	char fcos[10] = "cos";              //исправить левое количество памяти

	strcpy(functions[sin], fsin);
	strcpy(functions[cos], fcos);

	return functions;
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
	NODE* left_node = GetF();

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


NODE* GetF()
{
	NODE* left_node = GetP();

	int function = DetectFunction();

	if (function == sin)
	{
		s += 3;
		NODE* left_node = Create_node(-1, noth);

		NODE* new_node = Create_node(sin, func);

		new_node->left = left_node;
		new_node->right = GetP();

		return new_node;
	}

	if (function == cos)
	{
		s += 3;
		NODE* left_node = Create_node(-1, noth);

		NODE* new_node = Create_node(cos, func);

		new_node->left = left_node;
		new_node->right = GetP();

		return new_node;
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

int DetectFunction()
{
	if ((*s == 's') && (*(s + 1) == 'i') && (*(s + 2) == 'n'))
		return sin;

	if ((*s == 'c') && (*(s + 1) == 'o') && (*(s + 2) == 's'))
		return cos;

	return 0;
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

	char* commands = operations_array();
	
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

	if (node->type == func)
	{
		if (node->data == sin)
		{
			NODE* mul_node = Create_node(Mul, operation);

			mul_node->right = Differenciator(node->right);

			mul_node->left = Copy(node);

			mul_node->left->data = cos;

			return mul_node;
		}
	}
}

int PrintTree(NODE* node, FILE* output, char* operations, char** functions)
{
	if (priory_pro(node) > priory_pro(node->left))
		fprintf(output, "(");

	if (node->left)
		PrintTree(node->left, output, operations, functions);

	if (priory_pro(node) > priory_pro(node->left))
		fprintf(output, ")");

	PrintNode(node, output, operations, functions);

	if (priory_pro(node) > priory_pro(node->right))
		fprintf(output, "(");

	if (node->right)
		PrintTree(node->right, output, operations, functions);

	if (priory_pro(node) > priory_pro(node->right))
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

int PrintNode(NODE* node, FILE* output, char* operations, char** functions)
{
	if (node->type == num)
		fprintf(output, "%d", node->data);

	else if (node->type == operation)
		fprintf(output, "%c", operations[node->data]);

	else if (node->type == var)
		fprintf(output, "x");

	else if (node->type == func)
		fprintf(output, "%s", functions[node->data]);

	return 0;
}

int priory_pro(NODE* node)
{
	if (!node)
		return 10;

	if (node->type == operation)
	{
		switch (node->data) {
		case Add:
			return 1;
		case Sub:
			return 1;
		case Mul:
			return 2;
		case Div:
			return 2;
		case power:
			return 4;
		}
	}

	if (node->type == func)
		return 3;

	return 10;
}

int priory2(NODE* node)
{
	int res = 0;

	if (node->type != operation)
		return res;

	if (node->type == power)
		res = 11;

	if (node->type == func)
		res = 21;

	if ((node->data == Mul) || (node->data == Div))
	{
		if ((node->left->data == Add) || (node->left->data == Sub))
			res = 12;

		if ((node->right->data == Add) || (node->right->data == Sub))
		{
			if (res == 12)
				res = 11;
			else
				res = 21;
		}

		if ((node->right->type == operation) && (node->left->type == func))
			res = 12;

		//if ((node->left->data == Add) || (node->left->data == Sub))

	}

	return res;
}

int priory(NODE* node)
{
	int res = 0;

	if (node->data == power)
		return 31;

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
