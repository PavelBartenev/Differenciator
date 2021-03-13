#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
//#include <math.h>

enum error_codes
{
	calloc_fail = 1313,
	arg_fail = 1414,
	file_fail = 1515,
};

enum commands
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
	tan = 17,
	ln = 18,
};

struct NODE
{

public:
	int data;
	NODE* left;
	NODE* right;
	NODE* prev;
	int call;
	int type;

	NODE* Create_node(int data, int type)
	{
		NODE* new_node = (NODE*)calloc(1, sizeof(NODE));

		if (!new_node)
			return 0;

		new_node->data = data;

		new_node->type = type;

		return new_node;
	}

	NODE* operator + (NODE& other)
	{
		NODE* new_node = Create_node(Add, operation);

		new_node->right = &other;
		new_node->left = this;

		return new_node;
	}

	NODE* operator - (NODE& other)
	{
		NODE* new_node = Create_node(Sub, operation);

		new_node->right = &other;
		new_node->left = this;

		return new_node;
	}

	NODE* operator * (NODE& other)
	{
		NODE* new_node = Create_node(Mul, operation);

		new_node->right = &other;
		new_node->left = this;

		return new_node;
	}

	NODE* operator ^ (NODE& other)
	{
		NODE* new_node = Create_node(power, operation);

		new_node->right = &other;
		new_node->left = this;

		return new_node;
	}

};

int g_dump_work = 0;

#define DEBUG

#ifdef DEBUG
#define DUMP(code)																								  \
    {																											  \
       if (g_dump_work == 0)																				      \
	   {                                                                                                          \
               printf("FAILED  LINE %ld\nFUNCTION FAILED %s\n\n", __LINE__, __FUNCTION__);					      \
		       printf("CODE %d", code);                                                                           \
	   }																										  \
       g_dump_work++;                                                                                             \
    }                    																						  \

#else                                                                                                                                                        
#define DUMP(this_);      
#endif

const char* current_symbol = "";

int g_number = 0;

NODE* BuildTree(FILE*);

NODE* OptimizedOut(NODE*, FILE*);

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

int Draw_tree(NODE*, FILE*, int, char*, char**);

int Draw_node(NODE*, FILE*, char*, char**);

int Graphviz_prepare(NODE*, int);

char* operations_array();

char** functions_array();

NODE* Differenciator(NODE*);

NODE* add_sub_diff(NODE*);

NODE* mul_diff(NODE*);

NODE* power_diff(NODE*);

NODE* func_diff(NODE*);

int PrintTree(NODE*, FILE*, char*, char**);

int PrintNode(NODE*, FILE*, char*, char**);

int DetectFunction();

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

	NODE* head = BuildTree(input);

	NODE* diff = Differenciator(head);

	FILE* output = fopen("output.txt", "w");

	OptimizedOut(diff, output);

	Draw(diff);

	system("dot -Tpng D:\\vs_projects\\Calculator\\graph_code_test.txt -oD:\\vs_projects\\Calculator\\graph_image.png");

	return 0;
}

NODE* BuildTree(FILE* input)
{
	if (!input)
	{
		DUMP(file_fail);
		return 0;
	}

	fseek(input, 0, SEEK_END);
	int size = ftell(input);
	fseek(input, 0, SEEK_SET);

	char* str = (char*)calloc(size, sizeof(char));

	fscanf(input, "%s", str);

	NODE* head = GetG(str);

	return head;
}

NODE* OptimizedOut(NODE* diff, FILE* output)
{
	if (!diff)
	{
		DUMP(arg_fail);
		return 0;
	}

	if (!output)
	{
		DUMP(file_fail);
		return 0;
	}

	for (int i = 0; i < 3; i++)
		Optimizator(diff);
	   
	char* commands = operations_array();

	char** functions = functions_array();

	PrintTree(diff, output, commands, functions);

	return diff;
}

char* operations_array()
{
	char* operations = (char*)calloc(20, sizeof(char));

	if (!operations)
	{
		DUMP(calloc_fail);
		return 0;
	}

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
	char** functions = (char**)calloc(50, sizeof(char*));

	if (!functions)
	{
		DUMP(calloc_fail);
		return 0;
	}

	for (int i = 0; i < 50; i++)
	{
		functions[i] = (char*)calloc(10, sizeof(char));

		if (!functions[i])
		{
			DUMP(calloc_fail);
			return 0;
		}
	}
    
	char fsin[10] = "sin";
	char fcos[10] = "cos";
	char ftan[10] = "tan";                                    //исправить случайное количество памяти
	char fln[10] = "ln";

	strcpy(functions[sin], fsin);
	strcpy(functions[cos], fcos);
	strcpy(functions[tan], ftan);
	strcpy(functions[ln], fln);

	return functions;
}

NODE* GetG(char* str)
{
	current_symbol = str;
	NODE* head = GetE();
	assert(*current_symbol == '\0');

	return head;
}

NODE* GetE()
{
	NODE* left_node = GetT();

	while (*current_symbol == '+' || *current_symbol == '-')
	{
		char op = *current_symbol;
		current_symbol++;

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

	while (*current_symbol == '*' || *current_symbol == '/')
	{
		char op = *current_symbol;
		current_symbol++;

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

	if (*current_symbol == '^')
	{
		char op = *current_symbol;
		current_symbol++;

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
		current_symbol += 3;

		NODE* left_node = Create_node(-1, noth);

		NODE* new_node = Create_node(sin, func);

		new_node->left = left_node;
		new_node->right = GetP();

		return new_node;
	}

	if (function == cos)
	{
		current_symbol += 3;

		NODE* left_node = Create_node(-1, noth);

		NODE* new_node = Create_node(cos, func);

		new_node->left = left_node;
		new_node->right = GetP();

		return new_node;
	}

	if (function == tan)
	{
		current_symbol += 3;

		NODE* left_node = Create_node(-1, noth);

		NODE* new_node = Create_node(tan, func);

		new_node->left = left_node;
		new_node->right = GetP();

		return new_node;
	}

	if (function == ln)
	{
		current_symbol += 2;

		NODE* left_node = Create_node(-1, noth);

		NODE* new_node = Create_node(ln, func);

		new_node->left = left_node;
		new_node->right = GetP();

		return new_node;
	}

	return left_node;
}

NODE* GetP()
{
	if (*current_symbol == '(')
	{
		current_symbol++;
		NODE* inside = GetE();
		assert(*current_symbol == ')');
		current_symbol++;
		return inside;
	}

	else if (*current_symbol == 'x')
	{
		current_symbol++;
		return Create_node(varx, var);
	}

	else
		return GetN();
}

NODE* GetN()
{
	int val = 0;

	while ('0' <= *current_symbol && *current_symbol <= '9')
	{
		val = 10 * val + (*current_symbol - '0');
		current_symbol++;
	}

	return Create_node(val, num);
}

int DetectFunction()
{
	char* current_function = (char*)calloc(20, sizeof(char));

	if (!current_function)
		return 1;

	memcpy(current_function, current_symbol, 10);

	if (!strncmp(current_function, "sin", 3))
		return sin;

	if (!strncmp(current_function, "cos", 3))
		return cos;

	if (!strncmp(current_function, "tan", 3))
		return tan;

	if (!strncmp(current_function, "ln", 2))
		return ln;

	return 0;
}

NODE* Create_node(int data, int type)
{
	NODE* new_node = (NODE*)calloc(1, sizeof(NODE));

	if (!new_node)
	{
		DUMP(calloc_fail);
		return 0;
	}

	new_node->data = data;

	new_node->type = type;

	new_node->call = g_number;

	g_number++;

	return new_node;
}

int second_link(NODE* node, NODE* prev_node)
{
	if (!node || !prev_node)
	{
		DUMP(arg_fail);
		return 0;
	}

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

	char** functions = functions_array();
	
	Draw_param(node, output);
	Draw_tree(node, output, 1, commands, functions);
	fprintf(output, "}\n}");

	fclose(output);

	return 0;
}

int Draw_param(NODE* node, FILE* output)
{
	fprintf(output, "digraph MyPlan {\n");
	fprintf(output, "  node[shape = \"circle\", style = \"filled\", fillcolor = \"white\", fontcolor = \"#FFFFFF\", margin = \"0.01\"];\n");
	fprintf(output, "  edge [style=\"dashed\"];\n");

	fprintf(output, "{\n node[shape = \"plaintext\",style = \"invisible\"];\n edge[color = \"white\"];\n 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10 -> 11 -> 12 -> 13 -> 14;\n}\n");

	fprintf(output, "subgraph tree {\n   node [shape=\"ellipse\", style=\"filled\", fillcolor=\"white\", fontcolor=\"black\", fontsize=\"15\"];\n");

	return 0;
}

int Draw_tree(NODE* node, FILE* output, int level, char* commands, char** functions)
{
	if (node->left)
	{
		fprintf(output, "   { rank = \"same\"; \"%d\"; ", level);
		Draw_node(node, output, commands, functions);
		fprintf(output, "  ;}\n");
		Draw_node(node, output, commands, functions);
		fprintf(output, "->");
		Draw_node(node->left, output, commands, functions);
		fprintf(output, "  ;\n");

		Draw_tree(node->left, output, (level + 1), commands, functions);
	}


	if (node->right)
	{
		fprintf(output, "   { rank = \"same\"; \"%d\"; ", level);
		Draw_node(node, output, commands, functions);
		fprintf(output, "  ;}\n");
		Draw_node(node, output, commands, functions);
		fprintf(output, "->");
		Draw_node(node->right, output, commands, functions);
		fprintf(output, "  ;\n");

		Draw_tree(node->right, output, (level + 1), commands, functions);
	}

	return 0;
}

int Draw_node(NODE* node, FILE* output, char* commands, char** functions)
{
	if (node->type == num)
		fprintf(output, "   \"%d type = %p\" ", node->data, &(node->data));
	
	if (node->type == operation)
		fprintf(output, "   \"%c type = %p\" ", commands[node->data], &(node->data));

	if (node->type == func)
		fprintf(output, "   \"%s type = %p\" ", functions[node->data], &(node->data));

	if (node->type == noth)
		fprintf(output, "   \"nothing type = %p\" ", &(node->data));

	if (node->type == var)
		fprintf(output, "   \"x = %p\" ", &(node->data));

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
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

	if (node->type == var)
		return Create_node(1, num);
	
	if (node->type == num)
		return Create_node(0, num);

	if (node->type == operation)
	{
		if (node->data == Add || node->data == Sub)
			return add_sub_diff(node);

		if (node->data == Mul)
			return mul_diff(node);

		if (node->data == power)
			return power_diff(node);
	}

	if (node->type == func)
		return func_diff(node);

	return 0;
}

NODE* add_sub_diff(NODE* node)
{
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

	if (node->data == Add)
		return (*Differenciator(node->left) + *Differenciator(node->right));


	if (node->data == Sub)
		return (*Differenciator(node->left) - *Differenciator(node->right));

	return 0;
}

NODE* mul_diff(NODE* node)
{
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

	return (*(*Differenciator(node->left) * *Copy(node->right)) + *(*Copy(node->left) * *Differenciator(node->right)));
}

NODE* power_diff(NODE* node)
{
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

	if (node->right->type == num) 
		return *(*Create_node(node->right->data, num) * *(*Copy(node->left) ^ *Create_node(node->right->data - 1, num))) * (*Differenciator(node->left));

	else
	{
		NODE* log = Create_node(ln, func);
		log->right = Copy(node->right);
		log->left = Create_node(-1, noth);

		return (*Copy(node)) * *(*(*Differenciator(node->right) * *log) + *(*(*Differenciator(node->left) * *Copy(node->right)) * *(*Copy(node->left) ^ *Create_node(-1, num))));
	}
}

NODE* func_diff(NODE* node)
{
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

	if (node->data == sin)
	{
		NODE* high = *Copy(node) * *Differenciator(node->right);

		high->left->data = cos;

		return high;
	}

	if (node->data == cos)
	{
		NODE* high = *(*Create_node(-1, num) * *Copy(node)) * *Differenciator(node->right);

		high->left->right->data = sin;

		return high;
	}

	if (node->data == tan)
	{
		NODE* high = *(*Create_node(cos, func) ^ *Create_node(-2, num)) * *Differenciator(node->right);

		high->left->left->right = Copy(node->left);

		return high;
	}

	if (node->data == ln)
	{
		NODE* high = *(*Copy(node->right) ^ *Create_node(-1, num)) * *Differenciator(node->right);

		return high;
	}

	return 0;
}

int PrintTree(NODE* node, FILE* output, char* operations, char** functions)
{
	if (!node || !operations || !functions)
	{
		DUMP(arg_fail);
		return 0;
	}

	if (priory(node) > priory(node->left))
		fprintf(output, "(");

	if (node->left)
		PrintTree(node->left, output, operations, functions);

	if (priory(node) > priory(node->left))
		fprintf(output, ")");

	PrintNode(node, output, operations, functions);

	if (priory(node) > priory(node->right))
		fprintf(output, "(");

	if (node->right)
		PrintTree(node->right, output, operations, functions);

	if (priory(node) > priory(node->right))
		fprintf(output, ")");

	return 0;
}

NODE* Copy(NODE* node)
{
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

	NODE* new_node = Create_node(node->data, node->type);

	if (!new_node)
	{
		DUMP(calloc_fail);
		return 0;
	}

	if (node->left)
		new_node->left = Copy(node->left);

	if (node->right)
		new_node->right = Copy(node->right);

	return new_node;
}

int PrintNode(NODE* node, FILE* output, char* operations, char** functions)
{                          
	if (!node || !operations || !functions)
	{
		DUMP(arg_fail);
		return 0;
	}

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

int priory(NODE* node)
{
	if (!node)
		return 13;

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
			return 11;
		}
	}

	if (node->type == func)
		return 12;

	return 13;
}

int Optimizator(NODE* node)
{
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

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
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

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
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

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
	if (!node)
	{
		DUMP(arg_fail);
		return 0;
	}

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
