//
//  trans_def.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/27/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_LIB_TRANS_DEF_H_
#define YCSB_C_LIB_TRANS_DEF_H_

#define SVM_SIZE (0x200000000)

#define TRANS_BEGIN do
#define TRANS_END(svm) while (sitevm_commit_and_update(svm))

#endif // YCSB_C_LIB_TRANS_DEF_H_

