---
title: 377. Combination Sum IV
index_img: /Picture/Dynamic-programming.jpeg
date: 2020-11-14 23:38:27
tags:
- 力扣
- 动态规划
categories:
- 力扣
banner_img:
---
![377-Combination-Sum-IV-1.png](377-Combination-Sum-IV-1.png)<br>
![377-Combination-Sum-IV-2.png](377-Combination-Sum-IV-2.png)<br>

```java
class Solution {
    public int combinationSum4(int[] nums, int target) {
        int[] fina = new int[target+1];
        //到达target=0时到达target==0的方式有1种,就是什么都不选
        fina[0]=1;
        //fina[i]表示target=i时有几种方式(路)可以到达target=0.
        for(int i=1;i<=target;++i)
        {
            for(int j=0;j<nums.length;++j)
            {
                if(nums[j]<=i)
                fina[i]+=fina[i-nums[j]];
            }
        }
        return fina[target];
    }
}
```
