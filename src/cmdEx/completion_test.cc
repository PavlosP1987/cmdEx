// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cmdEx/completion.h"

#include "gtest/gtest.h"

// Some tests based on:
// http://source.winehq.org/git/wine.git/blob/HEAD:/dlls/shell32/tests/shlexec.c

TEST(CompletionTest, BreakIntoWordsNoArgs) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe", &words);
  EXPECT_EQ(1u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
}

TEST(CompletionTest, BreakIntoWordsBasicQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe a b \"c d\" 'e f` g\\ h)", &words);
  EXPECT_EQ(8u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"a", words[1].original_word);
  EXPECT_EQ(L"b", words[2].original_word);
  EXPECT_EQ(L"\"c d\"", words[3].original_word);
  EXPECT_EQ(L"'e", words[4].original_word);
  EXPECT_EQ(L"f`", words[5].original_word);
  EXPECT_EQ(L"g\\", words[6].original_word);
  EXPECT_EQ(L"h)", words[7].original_word);
}

TEST(CompletionTest, BreakIntoWordsQuotesInArgs) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe a\"b\" \"c\"d e", &words);
  EXPECT_EQ(4u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"a\"b\"", words[1].original_word);
  EXPECT_EQ(L"\"c\"d", words[2].original_word);
  EXPECT_EQ(L"e", words[3].original_word);
}

TEST(CompletionTest, BreakIntoWordsUnclosedQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe a \"b", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"a", words[1].original_word);
  EXPECT_EQ(L"\"b", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsStandaloneQuote) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe a \"", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"a", words[1].original_word);
  EXPECT_EQ(L"\"", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsOnlySpaceAndTab) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe a\tb\rc\nd\ve\ff", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"a", words[1].original_word);
  EXPECT_EQ(L"b\rc\nd\ve\ff", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsBackslashNoQuote) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe o\\ne t\\\\wo t\\\\\\hree f\\\\\\\\our",
                           &words);
  EXPECT_EQ(5u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"o\\ne", words[1].original_word);
  EXPECT_EQ(L"t\\\\wo", words[2].original_word);
  EXPECT_EQ(L"t\\\\\\hree", words[3].original_word);
  EXPECT_EQ(L"f\\\\\\\\our", words[4].original_word);
}

TEST(CompletionTest, BreakIntoWordsBackslashNoQuoteInQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(
      L"exe \"o\\ne\" \"t\\\\wo\" \"t\\\\\\hree\" \"f\\\\\\\\our\"", &words);
  EXPECT_EQ(5u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"\"o\\ne\"", words[1].original_word);
  EXPECT_EQ(L"\"t\\\\wo\"", words[2].original_word);
  EXPECT_EQ(L"\"t\\\\\\hree\"", words[3].original_word);
  EXPECT_EQ(L"\"f\\\\\\\\our\"", words[4].original_word);
}

TEST(CompletionTest, BreakIntoWordsBackslashQuote) {
  vector<WordData> words;
  CompletionBreakIntoWords(
      L"exe \\\"one \\\\\"two\" \\\\\\\"three \\\\\\\\\"four\" after", &words);
  EXPECT_EQ(6u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"\\\"one", words[1].original_word);
  EXPECT_EQ(L"\\\\\"two\"", words[2].original_word);
  EXPECT_EQ(L"\\\\\\\"three", words[3].original_word);
  EXPECT_EQ(L"\\\\\\\\\"four\"", words[4].original_word);
  EXPECT_EQ(L"after", words[5].original_word);
}

TEST(CompletionTest, BreakIntoWordsBackslashQuoteSpaced) {
  vector<WordData> words;
  CompletionBreakIntoWords(
      L"exe \"one\\\" still\" \"two\\\\\" \"three\\\\\\\" still\" " \
      L"\"four\\\\\\\\\" after",
      &words);
  EXPECT_EQ(6u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"\"one\\\" still\"", words[1].original_word);
  EXPECT_EQ(L"\"two\\\\\"", words[2].original_word);
  EXPECT_EQ(L"\"three\\\\\\\" still\"", words[3].original_word);
  EXPECT_EQ(L"\"four\\\\\\\\\"", words[4].original_word);
  EXPECT_EQ(L"after", words[5].original_word);
}

TEST(CompletionTest, BreakIntoWordsDoubleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe two\"\"quotes after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"two\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"twoquotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsTripleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe three\"\"\"quotes after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"three\"\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"three\"quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsQuadrupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe four\"\"\"\" quotes\" after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"four\"\"\"\" quotes\"", words[1].original_word);
  EXPECT_EQ(L"four\" quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsQuintupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe five\"\"\"\"\"quotes after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"five\"\"\"\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"five\"quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsHextupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe six\"\"\"\"\"\"quotes after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"six\"\"\"\"\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"six\"\"quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsSextupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe six\"\"\"\"\"\"quotes after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"six\"\"\"\"\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"six\"\"quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsSeptupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe seven\"\"\"\"\"\"\" quotes\" after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"seven\"\"\"\"\"\"\" quotes\"", words[1].original_word);
  EXPECT_EQ(L"seven\"\" quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsDuodectupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe twelve\"\"\"\"\"\"\"\"\"\"\"\"quotes after",
                           &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"twelve\"\"\"\"\"\"\"\"\"\"\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"twelve\"\"\"\"quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

// Had to bust out Wikipedia for that name.
TEST(CompletionTest, BreakIntoWordsTredectupleQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(
      L"exe thirteen\"\"\"\"\"\"\"\"\"\"\"\"\" quotes\" after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"thirteen\"\"\"\"\"\"\"\"\"\"\"\"\" quotes\"",
            words[1].original_word);
  EXPECT_EQ(L"thirteen\"\"\"\" quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
}

TEST(CompletionTest, BreakIntoWordsQuotedContainingTwoQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe \"two\"\"quotes after", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"\"two\"\"quotes", words[1].original_word);
  EXPECT_EQ(L"two\"quotes", words[1].deescaped_word);
  EXPECT_EQ(L"after", words[2].original_word);
  EXPECT_EQ(L"after", words[2].deescaped_word);
}

TEST(CompletionTest, BreakIntoWordsEscapedConsecutiveQuotes) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"exe \"the crazy \\\\\"\"\"\\\\\" quotes", &words);
  EXPECT_EQ(3u, words.size());
  EXPECT_EQ(L"exe", words[0].original_word);
  EXPECT_EQ(L"\"the crazy \\\\\"\"\"\\\\\"", words[1].original_word);
  EXPECT_EQ(L"the crazy \\\"\\", words[1].deescaped_word);
  EXPECT_EQ(L"quotes", words[2].original_word);
  EXPECT_EQ(L"quotes", words[2].deescaped_word);
}

TEST(CompletionTest, MergeStartingSlashWithTrailingQuote) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"\"C:\\Program Files (x86)\"\\", &words);
  EXPECT_EQ(1u, words.size());
  EXPECT_EQ(L"\"C:\\Program Files (x86)\"\\", words[0].original_word);
}

TEST(CompletionTest, Quoting) {
  EXPECT_EQ(L"stuff", QuoteWord(L"stuff"));
  EXPECT_EQ(L"\"st uff\"", QuoteWord(L"st uff"));
  EXPECT_EQ(L"\"\\\"st uff\\\"\"", QuoteWord(L"\"st uff\""));
  EXPECT_EQ(L"\"st uff\\\\\"", QuoteWord(L"st uff\\"));
}

TEST(CompletionTest, WordsToCommands) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"git && ninja && stuff", &words);
  vector<vector<WordData>> by_command = CompletionBreakWordsIntoCommands(words);
  EXPECT_EQ(3u, by_command.size());
  EXPECT_EQ(1u, by_command[0].size());
  EXPECT_EQ(L"git", by_command[0][0].original_word);
  EXPECT_EQ(1u, by_command[1].size());
  EXPECT_EQ(L"ninja", by_command[1][0].original_word);
  EXPECT_EQ(1u, by_command[2].size());
  EXPECT_EQ(L"stuff", by_command[2][0].original_word);
}

TEST(CompletionTest, WordsToCommandsQuoted) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"git && echo \"ninja with && stuff\"", &words);
  vector<vector<WordData>> by_command = CompletionBreakWordsIntoCommands(words);
  EXPECT_EQ(2u, by_command.size());
  EXPECT_EQ(1u, by_command[0].size());
  EXPECT_EQ(L"git", by_command[0][0].original_word);
  EXPECT_EQ(2u, by_command[1].size());
  EXPECT_EQ(L"echo", by_command[1][0].original_word);
  EXPECT_EQ(L"\"ninja with && stuff\"", by_command[1][1].original_word);
}

TEST(CompletionTest, WordsToCommandsVariousSeparators) {
  vector<WordData> words;
  CompletionBreakIntoWords(L"git || ninja & stuff && wee", &words);
  vector<vector<WordData>> by_command = CompletionBreakWordsIntoCommands(words);
  EXPECT_EQ(4u, by_command.size());
  EXPECT_EQ(1u, by_command[0].size());
  EXPECT_EQ(L"git", by_command[0][0].original_word);
  EXPECT_EQ(1u, by_command[1].size());
  EXPECT_EQ(L"ninja", by_command[1][0].original_word);
  EXPECT_EQ(1u, by_command[2].size());
  EXPECT_EQ(L"stuff", by_command[2][0].original_word);
  EXPECT_EQ(1u, by_command[3].size());
  EXPECT_EQ(L"wee", by_command[3][0].original_word);
}

// TODO
// - special executable handling
// - ^ escape not handled
