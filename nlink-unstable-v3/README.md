```
$ nlink --interactive

*******************************************
*         NexusLink CLI System           *
*          © OBINexus Computing          *
*******************************************
Type 'help' for available commands, 'exit' to quit

nexus> load tokenizer
Loading component 'tokenizer'...
Successfully loaded component 'tokenizer'

nexus> load parser
Loading component 'parser'...
Successfully loaded component 'parser'

nexus> pipeline create mode=single
Created pipeline in single-pass mode with optimization enabled

nexus> pipeline add-stage tokenizer
Added stage 'tokenizer' to pipeline

nexus> pipeline add-stage parser
Added stage 'parser' to pipeline

nexus> pipeline execute
Running pipeline optimizations...
Executing pipeline...
Pipeline executed successfully in 45.23 ms with 1 iteration(s)

nexus> stats
System Statistics:
-----------------
  Components loaded: 2
  Memory usage: 0.8 MB
  Heap allocations: 73
  Peak memory: 1.2 MB
  Symbol table entries: 128
  Commands registered: 7
  Pipelines active: 1

nexus> exit
```

Now, here's the equivalent non-interactive script file (`script.nlink`):

```
# script.nlink - Tokenizer/Parser single pass pipeline
# Load required components
load tokenizer
load parser

# Create a single-pass pipeline
pipeline create mode=single

# Add processing stages in sequence
pipeline add-stage tokenizer
pipeline add-stage parser

# Execute the pipeline
pipeline execute

# Display statistics
stats
```

You would execute this script with:

```
$ nlink --execute script.nlink
```

This would produce output similar to the interactive session but without the prompts:

```
Loading component 'tokenizer'...
Successfully loaded component 'tokenizer'
Loading component 'parser'...
Successfully loaded component 'parser'
Created pipeline in single-pass mode with optimization enabled
Added stage 'tokenizer' to pipeline
Added stage 'parser' to pipeline
Running pipeline optimizations...
Executing pipeline...
Pipeline executed successfully in 43.88 ms with 1 iteration(s)
System Statistics:
-----------------
  Components loaded: 2
  Memory usage: 0.8 MB
  Heap allocations: 73
  Peak memory: 1.2 MB
  Symbol table entries: 128
  Commands registered: 7
  Pipelines active: 1
```

The system is executing a single-pass pipeline where input data flows through the tokenizer component first (which breaks input into tokens) and then through the parser component (which constructs a syntactic structure from those tokens). The single-pass mode ensures data flows through each component exactly once in the defined sequence.

see docs/usage/*.md 