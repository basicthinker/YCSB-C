//
//  mem_allocator.h
//
//  Created by Jinglei Ren on 12/23/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef VM_PERSISTENCE_MEM_ALLOCATOR_H_
#define VM_PERSISTENCE_MEM_ALLOCATOR_H_

#ifdef SVM
#include "sitevm/sitevm_malloc.h"
#define MALLOC(n) sitevm_malloc::smalloc(n)
#define FREE(p) sitevm_malloc::sfree((void *)(p))
#else
#define MALLOC(n) malloc(n)
#define FREE(p) free((void *)(p))
#endif

#define NEW(type, ...) ({ \
      type *p = (type *)MALLOC(sizeof(type)); \
      ::new((void*)(p)) (type)(__VA_ARGS__); \
      p; \
    })

#define DELETE(type, p) { \
      (p)->~type(); \
      FREE(p); \
    }

#endif // VM_PERSISTENCE_MEM_ALLOCATOR_H_

