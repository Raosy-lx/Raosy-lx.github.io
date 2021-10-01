# Elastic Search学习笔记

参考：

[elasticsearch_百度百科](https://baike.baidu.com/item/elasticsearch/3411206?fr=aladdin)

[ElasticSearch原理 - 神一样的存在 - 博客园](https://www.cnblogs.com/dreamroute/p/8484457.html)

### 1、ElasticSearch简介

- Elasticsearch是一个基于[Lucene](https://baike.baidu.com/item/Lucene/6753302)的搜索服务器，一个分布式、高扩展、高实时的搜索与数据分析引擎。它提供了一个分布式多用户能力的[全文搜索引擎](https://baike.baidu.com/item/%E5%85%A8%E6%96%87%E6%90%9C%E7%B4%A2%E5%BC%95%E6%93%8E/7847410)，基于RESTful web接口。

- Elasticsearch 并不仅仅是 Lucene 那么简单，它不仅包括了全文搜索功能，还可以进行以下工作:
  
  - 分布式实时文件存储，并将每一个字段都编入索引，使其可以被搜索。
  
  - 实时分析的分布式搜索引擎。
  
  - 可以扩展到上百台服务器，处理**PB级别**的结构化或非结构化数据。

- Elasticsearch是与名为**Logstash**的**数据收集和日志解析引擎**以及名为**Kibana**的**分析和可视化平台**一起开发的。这三个产品被设计成一个集成解决方案，称为“Elastic Stack”（以前称为“ELK stack”）。

### 2、ElasticSearch原理

- 首先用户将数据提交到**Elasticsearch** **数据库**中

- 再通过**分词**控制器去将对应的语句分词，将其权重和分词结果一并存入数据

- 当用户搜索数据时候，再根据权重将**结果排名，打分**，再将返回结果呈现给用户。

**核心要点：** 倒排索引

###### 2.1、ElasticSearch和关系数据库的对比

```textile
关系数据库 ⇒ 数据库(Database) ⇒ 表(Table) ⇒ 行或者记录(Record)⇒ 列(Columns)
Elasticsearch ⇒ 索引(Index)     ⇒ 类型(type) ⇒ 文档(Docments) ⇒ 字段(Fields)
```

数据库行索引对应ES中的文档ID

一个 Elasticsearch 集群可以包含多个索引(数据库)，也就是说其中包含了很多类型(表)。这些类型中包含了很多的文档(行)，然后每个文档中又包含了很多的字段(列)。

###### 2.2、ElasticSearch文件存储

面向文档型数据库，一条数据一个文档，用**JSON**作为文档序列化格式

```json
{
    "name" :     "John",
    "sex" :      "Male",
    "age" :      25,
    "birthDate": "1990/05/01",
    "about" :    "I love to go rock climbing",
    "interests": [ "sports", "music" ]
}
```

###### 2.3、ElasticSearch基本操作

可以使用Java API，也可以直接使用**HTTP的Restful API**方式

```json
PUT /megacorp/employee/1  
{
    "name" :     "John",
    "sex" :      "Male",
    "age" :      25,
    "about" :    "I love to go rock climbing",
    "interests": [ "sports", "music" ]
}
```

PUT一个json的对象，即往Elasticsearch里插入一条记录，这个对象有多个fields。

在插入这些数据到Elasticsearch的同时，Elasticsearch还为这些字段建立**倒排索引**，因为Elasticsearch最核心功能是搜索。

###### 倒排索引

- 倒排索引又称反向索引，正向索引即通过文档ID可以定位到文档，进而获取文档中的单词，反向则是通过单词去检索包含该单词的文档的文档ID。

- 文档中的字段，记作Term，文档ID记作Posting List。

- Term Dictionary： elasticsearch为快速找term，为所有term进行排序，可通过二分查找term，与B-树类似，但更快，在于其想通过内存查找，不读磁盘，考虑内存空间有限，Term Index被采用。Term Index可以看作一颗树。

- 结合有限状态机FST，构建前缀树，定位到dictionary中的offset

###### 联合索引

单field索引，通过跳表或者Bitmap实现，多field联合索引，bitset按位与，跳表则是对最短posting list中每个id，逐个在其他posting list中查找是否有交集

### 3、ElasticSearch的部署及使用

< 国庆后补上 >!
