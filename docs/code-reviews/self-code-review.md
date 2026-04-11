# Self Code Review

Code Review 可以確保程式碼品質，也就是**可讀性、可維護姓、格式的一次檢查**，對於團隊可以塑造團隊文化，也是資深工程師協助初階工程師的好時機。(根據 [E+如何做好 Code Review? 如何寫出更快通過 Code Review 的程式碼?](https://www.explainthis.io/zh-hant/e-plus/blog/code-review))

當然，只有自己一個人也可以做 Code Review，而重點會放在程式碼品質上。

## 程式碼風格

### 變數命名

| 使用時機 | 命名法 | 說明 |
| - | - | - |
| 一般變數與函式 | `lowerPascalCase` | 單字組合時第一單字小寫，其餘單字開頭大寫 |
| `class`、`enum`、`structure` | `PascalCase` | 單字組合開頭大寫 |
| 常數 (`const`) | `UPPER_SNAKE_CASE` | 單字組合所有字皆大寫，用 `_` 分隔。 |

### 註解要求

- 若程式碼長到無法一眼看出功能，則須寫上註解。
- 若發現錯誤或警告，以 `ERROR` `WARN` 表示，如此之後即可 CTRL-F 搜尋。
