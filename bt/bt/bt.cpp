#include <iostream>

#include "Builder.h"

void BuilderTest1()
{
	std::cout << "BuilderTest1 begin" << "\n";
	try
	{
		Target t1{ 1, [] {std::cout << "BuilderTest1 Task 1" << "\n"; } };
		Target t2{ 2, [] {std::cout << "BuilderTest1 Task 2" << "\n"; } };
		Target t3{ 3, [] {std::cout << "BuilderTest1 Task 3" << "\n"; } };
		Target t4{ 4, [] {std::cout << "BuilderTest1 Task 4" << "\n"; } };
		Target t5{ 5, [] {std::cout << "BuilderTest1 Task 5" << "\n"; } };
		BuildGraph bg;
		bg.AddTarget(t1);
		bg.AddTarget(t2);
		bg.AddTarget(t3);
		bg.AddTarget(t4);
		bg.AddTarget(t5);
		bg.AddDependency(1, 2);
		bg.AddDependency(2, 3);
		bg.AddDependency(1, 4);
		bg.AddDependency(4, 5);
		Builder b(2);
		b.execute(bg, 1);
	}
	catch (const std::exception& ex)
	{
		std::cout << "BuilderTest1: exception. " << ex.what() << "\n";
	}
	std::cout << "BuilderTest1 end" << "\n";
};

void BuilderTest2()
{
	std::cout << "BuilderTest2 begin" << "\n";
	try
	{
		Target t1{ 1, [] {std::cout << "BuilderTest2 Task 1" << "\n"; } };
		Target t2{ 2, [] {std::cout << "BuilderTest2 Task 2" << "\n"; } };
		Target t3{ 3, [] {std::cout << "BuilderTest2 Task 3" << "\n"; } };
		Target t4{ 4, [] {std::cout << "BuilderTest2 Task 4" << "\n"; } };
		Target t5{ 5, [] {std::cout << "BuilderTest2 Task 5" << "\n"; } };
		BuildGraph bg;
		bg.AddTarget(t1);
		bg.AddTarget(t2);
		bg.AddTarget(t3);
		bg.AddTarget(t4);
		bg.AddTarget(t5);
		bg.AddDependency(1, 2);
		bg.AddDependency(2, 3);
		bg.AddDependency(3, 4);
		bg.AddDependency(4, 2);
		Builder b(2);
		b.execute(bg, 1);
	}
	catch (const std::exception& ex)
	{
		std::cout << "BuilderTest2: exception. " << ex.what() << "\n";
	}
	std::cout << "BuilderTest2 end" << "\n";
}

int main()
{
	BuilderTest1();
	//BuilderTest2();
}
