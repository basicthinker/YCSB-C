Structure of hash table classes
--> use
--- inherit

StringHashtable --> String
      |
      |--- StlHashtable --> std::unordered_map
      |        |
      |        |--- LockStlHashtable --> std::mutex
      |        |
      |    SvmHashtable --> SvmAllocator
      |
      |--- SlibHashtable --> slib::hashtable
      |        |
      |        |--- LockSlibHashtable --> std::mutex
      |        |
      |        |<----------|
      |                    |
      |------------ ItmSlibHashtable --> __transaction_atomic
      |
      |--- TbbRandHashtable --> tbb::concurrent_hash_map
      |
      |--- TbbScanHashtable --> tbb::concurrent_unordered_map

