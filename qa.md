# 常见问题

1. 为什么会报错 error LNK2001: 无法解析的外部符号 __imp___std_init_once_begin_initialize
				webrtc.lib(implicit_weak_message.obj) : error LNK2001: 无法解析的外部符号 __imp___std_init_once_complete
				webrtc.lib(auto_correlation.obj) : error LNK2001: 无法解析的外部符号 __std_reverse_copy_trivially_copyable_4？

    vs版本要为vs2019的16.10.3版本 且windows Sdk为 10.0.19041.0

