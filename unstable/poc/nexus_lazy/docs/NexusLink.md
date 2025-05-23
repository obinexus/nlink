flowchart TB
    subgraph "NexusLink Runtime"
        NL[NexusLink Core] --> SL[Symbol Loader]
        NL --> DR[Dependency Resolver]
        NL --> CM[Component Manager]
        
        SL --> ST[Symbol Table]
        DR --> DG[Dependency Graph]
        CM --> CR[Component Registry]
    end
    
    subgraph "Application"
        MP[Main Program] --> NL
        CF[Cold Functions] -.-> NL
    end
    
    subgraph "File System"
        SO[Shared Objects] -.-> CM
    end
    
    MP -- "1. Calls lazy function" --> NL
    NL -- "2. Checks if loaded" --> CM
    CM -- "3. Loads if needed" --> SO
    SO -- "4. Returns shared object" --> CM
    CM -- "5. Resolves symbols" --> SL
    SL -- "6. Updates symbol table" --> ST
    NL -- "7. Returns function ptr" --> MP
    MP -- "8. Executes function" --> CF
    
    classDef core fill:#f96,stroke:#333,stroke-width:2px;
    classDef app fill:#6f9,stroke:#333,stroke-width:2px;
    classDef fs fill:#69f,stroke:#333,stroke-width:2px;
    
    class NL,SL,DR,CM core;
    class MP,CF app;
    class SO fs;