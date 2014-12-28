//
//  trans_def.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/27/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_TRANS_DEF_H_
#define YCSB_C_TRANS_DEF_H_

#ifdef SVM
#define SVM_SIZE (0x200000000)
#endif

#ifdef SVM
#define TRANS_BEGIN do
#define TRANS_END(svm) while (sitevm_commit_and_update(svm))
#else
#define TRANS_BEGIN
#define TRANS_END
#endif

#endif // YCSB_C_TRANS_DEF_H_
