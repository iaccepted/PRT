# PRT
precomputed radiance transfer

分别将光照分布和mesh表面的transfer  通过球谐基函数来拟合，此部分通过预计算得到系数，然后根据球谐函数的性质来进行快速实时的渲染全局光照效果。


运行程序为了快速查看效果默认加载已经计算好的demo参数，直接渲染出结果。
如果要加载其他模型，请设置正确的模型路径，并修改main函数，计算新模型的参数信息，可以开启scene.saveAllData()来自动保存计算完成后的参数，以后可以直接scene.loadAllData()来进行加载，而无需重复计算。


默认为unshadowed模式，按下数字键2切换到shadowed模式，然后按1切换回unshadowed模式

![image](https://github.com/iaccepted/PRT/blob/master/screenshot/result.jpg)

