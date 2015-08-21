---
layout: post
title: Basic LLVM Analysis Passes
date: 2015-08-20
categories: c++ llvm
---

~~Basic Analysis Passes in LLVM INCOMPLETE~~
=============================

I did not find many tutorials or examples explaining this topic to me(a noob in compilers).
This Post is part of my attempt to use and understant llvm inbuilt AliasAnalysis and
MemoryDependenceAnalysis Passes.

###Alias Analysis

####Basic Theory

The main goal of alias analysis is to find if two pointers access i.e., point to same area of
memory. The result is *Must*, *Does Not*, or *May be*. *May be* may indicate that it depends on run
time variables and cannot be accurately determinded at compile time.


####Practice
In LLVM the passes which deal with alias analysis use llvm/Analysis/AliasAnalysis.h or
MemoryDependenceAnalysis.h sources. When -basic-aa or any other modref version pass is passed when
executed gathers the analysis data which can be used in other passes with getAnalysisUsage or some
such function.

###References
[llvm-aliasanalysis]:
[llvm-memdepanalysis]:
[llvm-someotheranalysis]:
