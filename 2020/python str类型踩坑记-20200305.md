# python string类型踩坑记

非专业`python`程序员小张今天用`python`写了个脚本，不出所料又出岔子了→.→

* ### 问题描述

  问题起源两个变量的对比

  ```python
  str1 = b'abcd'
  str2 = 'abcd'
  ```

  str1类型是bytes，str2类型是string，之前写`golang`对于[]byte和string类型基本可以等同对待，所以我天真的以为`python` string的底层是bytes，于是写下了这行代码

  ```python
  if str(str1) == str2 :
  		#do something
  ```

  显然我真的太天真了。。。

* ### 问题分析

  先来看看`golang`类似情况的处理

  ```go
  	var bf bytes.Buffer
  	bf.WriteByte('a')
  	var b []byte
  	b = append(b, 'a')
  	var str string
  	str = "a"
  	fmt.Println(str == bf.String())
  	fmt.Println(str == string(b))
  	fmt.Println(string(b) == bf.String())
  ```

  Output:

  ```go
  true
  true
  true
  ```

  后来我去了解了下，`golang`里的string也不是简单的等于[]byte，这里不做深入讨论

  对于python`

  python2[官方文档](https://docs.python.org/2/reference/lexical_analysis.html#string-literals)对string类型有如下说明：

  ```js
  * The backslash (\) character is used to escape characters that otherwise have a special meaning, such as newline, backslash itself, or the quote character.
  * String literals may optionally be prefixed with a letter 'r' or 'R'; such strings are called raw strings and use different rules for interpreting backslash escape sequences. A prefix of 'u' or 'U' makes the string a Unicode string. 
  * A prefix of 'b' or 'B' is ignored in Python 2; it indicates that the literal should become a bytes literal in Python 3 (e.g. when code is automatically converted with 2to3). A 'u' or 'b' prefix may be followed by an 'r' prefix.
  ```

  