#include "kevlr/include/lr.h"

#include <time.h>

bool conflict_report(void* object, KlrConflict* conflict, KlrCollection* collec) {
  printf("%d, %d: All conflict items here:\n", conflict->itemset->id, conflict->symbol->id);
  klr_print_itemset(stdout, collec, conflict->conflict_items, false);
  return false;
}

int main(int argc, char** argv) {
  KlrSymbol* end = klr_symbol_create(KLR_TERMINAL, "$");
  KlrSymbol* id = klr_symbol_create(KLR_TERMINAL, "id");
  KlrSymbol* plus = klr_symbol_create(KLR_TERMINAL, "+");
  KlrSymbol* minus = klr_symbol_create(KLR_TERMINAL, "-");
  KlrSymbol* mul = klr_symbol_create(KLR_TERMINAL, "*");
  KlrSymbol* div = klr_symbol_create(KLR_TERMINAL, "/");
  KlrSymbol* op5 = klr_symbol_create(KLR_TERMINAL, "|");
  KlrSymbol* op6 = klr_symbol_create(KLR_TERMINAL, "&");
  KlrSymbol* op7 = klr_symbol_create(KLR_TERMINAL, "^");
  KlrSymbol* op8 = klr_symbol_create(KLR_TERMINAL, "||");
  KlrSymbol* op9 = klr_symbol_create(KLR_TERMINAL, "&&");
  KlrSymbol* op10 = klr_symbol_create(KLR_TERMINAL, "<<");
  KlrSymbol* op11 = klr_symbol_create(KLR_TERMINAL, ">>");
  KlrSymbol* op12 = klr_symbol_create(KLR_TERMINAL, "<");
  KlrSymbol* op13 = klr_symbol_create(KLR_TERMINAL, ">");
  KlrSymbol* op14 = klr_symbol_create(KLR_TERMINAL, ">=");
  KlrSymbol* op15 = klr_symbol_create(KLR_TERMINAL, "<=");
  KlrSymbol* op16 = klr_symbol_create(KLR_TERMINAL, "==");
  KlrSymbol* op17 = klr_symbol_create(KLR_TERMINAL, "!=");
  KlrSymbol* op18 = klr_symbol_create(KLR_TERMINAL, "=");
  KlrSymbol* op19 = klr_symbol_create(KLR_TERMINAL, "->");
  KlrSymbol* op20 = klr_symbol_create(KLR_TERMINAL, "[");
  KlrSymbol* op21 = klr_symbol_create(KLR_TERMINAL, "]");
  KlrSymbol* op22 = klr_symbol_create(KLR_TERMINAL, ".");
  KlrSymbol* op23 = klr_symbol_create(KLR_TERMINAL, "++");
  KlrSymbol* op24 = klr_symbol_create(KLR_TERMINAL, "--");
  KlrSymbol* op25 = klr_symbol_create(KLR_TERMINAL, "!");
  KlrSymbol* op26 = klr_symbol_create(KLR_TERMINAL, "~");
  KlrSymbol* op27 = klr_symbol_create(KLR_TERMINAL, "&");
  KlrSymbol* openp = klr_symbol_create(KLR_TERMINAL, "(");
  KlrSymbol* closep = klr_symbol_create(KLR_TERMINAL, ")");
  KlrSymbol* E = klr_symbol_create(KLR_NONTERMINAL, "E");

  KlrSymbol* body1[] = { E, plus, E };
  KlrSymbol* body2[] = { E, minus, E };
  KlrSymbol* body3[] = { E, mul, E };
  KlrSymbol* body4[] = { E, div, E };
  KlrSymbol* body5[] = { plus, E };
  KlrSymbol* body6[] = { minus, E };
  KlrSymbol* body7[] = { openp, E, closep };
  KlrSymbol* body8[] = { id };
  KlrSymbol* body9[] =  { E, op5, E };
  KlrSymbol* body10[] = { E, op6, E };
  KlrSymbol* body11[] = { E, op7, E };
  KlrSymbol* body12[] = { E, op8, E };
  KlrSymbol* body13[] = { E, op9, E };
  KlrSymbol* body14[] = { E, op10, E };
  KlrSymbol* body15[] = { E, op11, E };
  KlrSymbol* body16[] = { E, op12, E };
  KlrSymbol* body17[] = { E, op13, E };
  KlrSymbol* body18[] = { E, op14, E };
  KlrSymbol* body19[] = { E, op15, E };
  KlrSymbol* body20[] = { E, op16, E };
  KlrSymbol* body21[] = { E, op17, E };
  KlrSymbol* body22[] = { E, op18, E };
  KlrSymbol* body23[] = { E, op19, E };
  KlrSymbol* body24[] = { E, op20, E, op21 };
  KlrSymbol* body25[] = { E, op22, E };
  KlrSymbol* body26[] = { E, op23 };
  KlrSymbol* body27[] = { E, op24 };
  KlrSymbol* body28[] = { op23, E };
  KlrSymbol* body29[] = { op24, E };
  KlrSymbol* body30[] = { op25, E };
  KlrSymbol* body31[] = { op26, E };
  KlrSymbol* body32[] = { op27, E };

  KlrRule* rule1 = klr_rule_create(E, body1, (sizeof body1 / sizeof body1[0]));
  KlrRule* rule2 = klr_rule_create(E, body2, (sizeof body2 / sizeof body2[0]));
  KlrRule* rule3 = klr_rule_create(E, body3, (sizeof body3 / sizeof body3[0]));
  KlrRule* rule4 = klr_rule_create(E, body4, (sizeof body4 / sizeof body4[0]));
  KlrRule* rule5 = klr_rule_create(E, body5, (sizeof body5 / sizeof body5[0]));
  KlrRule* rule6 = klr_rule_create(E, body6, (sizeof body6 / sizeof body6[0]));
  KlrRule* rule7 = klr_rule_create(E, body7, (sizeof body7 / sizeof body7[0]));
  KlrRule* rule8 = klr_rule_create(E, body8, (sizeof body8 / sizeof body8[0]));
  KlrRule* rule9 = klr_rule_create(E, body9, (sizeof body9 / sizeof body9[0]));
  KlrRule* rule10 = klr_rule_create(E, body10, (sizeof body10 / sizeof body10[0]));
  KlrRule* rule11 = klr_rule_create(E, body11, (sizeof body11 / sizeof body11[0]));
  KlrRule* rule12 = klr_rule_create(E, body12, (sizeof body12 / sizeof body12[0]));
  KlrRule* rule13 = klr_rule_create(E, body13, (sizeof body13 / sizeof body13[0]));
  KlrRule* rule14 = klr_rule_create(E, body14, (sizeof body14 / sizeof body14[0]));
  KlrRule* rule15 = klr_rule_create(E, body15, (sizeof body15 / sizeof body15[0]));
  KlrRule* rule16 = klr_rule_create(E, body16, (sizeof body16 / sizeof body16[0]));
  KlrRule* rule17 = klr_rule_create(E, body17, (sizeof body17 / sizeof body17[0]));
  KlrRule* rule18 = klr_rule_create(E, body18, (sizeof body18 / sizeof body18[0]));
  KlrRule* rule19 = klr_rule_create(E, body19, (sizeof body19 / sizeof body19[0]));
  KlrRule* rule20 = klr_rule_create(E, body20, (sizeof body20 / sizeof body20[0]));
  KlrRule* rule21 = klr_rule_create(E, body21, (sizeof body21 / sizeof body21[0]));
  KlrRule* rule22 = klr_rule_create(E, body22, (sizeof body22 / sizeof body22[0]));
  KlrRule* rule23 = klr_rule_create(E, body23, (sizeof body23 / sizeof body23[0]));
  KlrRule* rule24 = klr_rule_create(E, body24, (sizeof body24 / sizeof body24[0]));
  KlrRule* rule25 = klr_rule_create(E, body25, (sizeof body25 / sizeof body25[0]));
  KlrRule* rule26 = klr_rule_create(E, body26, (sizeof body26 / sizeof body26[0]));
  KlrRule* rule27 = klr_rule_create(E, body27, (sizeof body27 / sizeof body27[0]));
  KlrRule* rule28 = klr_rule_create(E, body28, (sizeof body28 / sizeof body28[0]));
  KlrRule* rule29 = klr_rule_create(E, body29, (sizeof body29 / sizeof body29[0]));
  KlrRule* rule30 = klr_rule_create(E, body30, (sizeof body30 / sizeof body30[0]));
  KlrRule* rule31 = klr_rule_create(E, body31, (sizeof body31 / sizeof body31[0]));
  KlrRule* rule32 = klr_rule_create(E, body32, (sizeof body32 / sizeof body32[0]));

  end->id = 0;
  id->id = 1;
  plus->id = 2;
  minus->id = 3;
  mul->id = 4;
  div->id = 5;
  op5->id = 6;
  op6->id = 7;
  op7->id = 8;
  op8->id = 9;
  op9->id = 10;
  op10->id = 11;
  op11->id = 12;
  op12->id = 13;
  op13->id = 14;
  op14->id = 15;
  op15->id = 16;
  op16->id = 17;
  op17->id = 18;
  op18->id = 19;
  op19->id = 20;
  op20->id = 21;
  op21->id = 22;
  op22->id = 23;
  op23->id = 24;
  op24->id = 25;
  op25->id = 26;
  op26->id = 27;
  op27->id = 28;
  openp->id = 29;
  closep->id = 30;
  E->id = 31;

  rule1->id = 0;
  rule2->id = 1;
  rule3->id = 2;
  rule4->id = 3;
  rule5->id = 4;
  rule6->id = 5;
  rule7->id = 6;
  rule8->id = 7;
  rule9->id = 8;
  rule10->id = 9;
  rule11->id = 10;
  rule12->id = 11;
  rule13->id = 12;
  rule14->id = 13;
  rule15->id = 14;
  rule16->id = 15;
  rule17->id = 16;
  rule18->id = 17;
  rule19->id = 18;
  rule20->id = 19;
  rule21->id = 20;
  rule22->id = 21;
  rule23->id = 22;
  rule24->id = 23;
  rule25->id = 24;
  rule26->id = 25;
  rule27->id = 26;
  rule28->id = 27;
  rule29->id = 28;
  rule30->id = 29;
  rule31->id = 30;
  rule32->id = 31;

  clock_t t = clock();
  for (size_t i = 0; i < 1000; ++i) {
    KlrCollection* collec = klr_collection_create_lalr(E, &end, 1);
    //klr_print_collection(stdout, collec, true);
    //KlrConflictHandler handler = { NULL, conflict_report };

    //KlrTable* table = klr_table_create(collec, NULL);
    //klr_print_trans_table(stdout, table);
    //klr_print_action_table(stdout, table);
    //klr_table_delete(table);
    klr_collection_delete(collec);
  }
  printf("total time used:%fs\n", ((float)(clock() - t)) / (float)CLOCKS_PER_SEC);

  klr_symbol_delete(end);
  klr_symbol_delete(id);
  klr_symbol_delete(plus);
  klr_symbol_delete(minus);
  klr_symbol_delete(mul);
  klr_symbol_delete(div);
  klr_symbol_delete(openp);
  klr_symbol_delete(closep);
  klr_symbol_delete(E);
  klr_symbol_delete(op5);
  klr_symbol_delete(op6);
  klr_symbol_delete(op7);
  klr_symbol_delete(op8);
  klr_symbol_delete(op9);
  klr_symbol_delete(op10);
  klr_symbol_delete(op11);
  klr_symbol_delete(op12);
  klr_symbol_delete(op13);
  klr_symbol_delete(op14);
  klr_symbol_delete(op15);
  klr_symbol_delete(op16);
  klr_symbol_delete(op17);
  klr_symbol_delete(op18);
  klr_symbol_delete(op19);
  klr_symbol_delete(op20);
  klr_symbol_delete(op21);
  klr_symbol_delete(op22);
  klr_symbol_delete(op23);
  klr_symbol_delete(op24);
  klr_symbol_delete(op25);
  klr_symbol_delete(op26);
  klr_symbol_delete(op27);

  klr_rule_delete(rule1);
  klr_rule_delete(rule2);
  klr_rule_delete(rule3);
  klr_rule_delete(rule4);
  klr_rule_delete(rule5);
  klr_rule_delete(rule6);
  klr_rule_delete(rule7);
  klr_rule_delete(rule8);
  klr_rule_delete(rule9);
  klr_rule_delete(rule10);
  klr_rule_delete(rule11);
  klr_rule_delete(rule12);
  klr_rule_delete(rule13);
  klr_rule_delete(rule14);
  klr_rule_delete(rule15);
  klr_rule_delete(rule16);
  klr_rule_delete(rule17);
  klr_rule_delete(rule18);
  klr_rule_delete(rule19);
  klr_rule_delete(rule20);
  klr_rule_delete(rule21);
  klr_rule_delete(rule22);
  klr_rule_delete(rule23);
  klr_rule_delete(rule24);
  klr_rule_delete(rule25);
  klr_rule_delete(rule26);
  klr_rule_delete(rule27);
  klr_rule_delete(rule28);
  klr_rule_delete(rule29);
  klr_rule_delete(rule30);
  klr_rule_delete(rule31);
  klr_rule_delete(rule32);



  //KlrSymbol* S = klr_symbol_create(KLR_NONTERMINAL, "S");
  //KlrSymbol* L = klr_symbol_create(KLR_NONTERMINAL, "L");
  //KlrSymbol* R = klr_symbol_create(KLR_NONTERMINAL, "R");
  //KlrSymbol* id = klr_symbol_create(KLR_TERMINAL, "id");
  //KlrSymbol* star = klr_symbol_create(KLR_TERMINAL, "*");
  //KlrSymbol* assign = klr_symbol_create(KLR_TERMINAL, "=");
  //KlrSymbol* end = klr_symbol_create(KLR_TERMINAL, "$");
  //KlrSymbol* body1[3] = { L, assign, R };
  //KlrSymbol* body2[2] = { star, R };
  //KlrRule* rule1 = klr_rule_create(S, &R, 1);
  //KlrRule* rule2 = klr_rule_create(L, &id, 1);
  //KlrRule* rule3 = klr_rule_create(R, &L, 1);
  //KlrRule* rule4 = klr_rule_create(S, body1, 3);
  //KlrRule* rule5 = klr_rule_create(L, body2, 2);
  //KlrRule* rule6 = klr_rule_create(R, NULL, 0);
  //KlrCollection* collec = klr_collection_create_slr(S, &end, 1);
  //
  //KevBitSet** follows = klr_util_compute_follows(collec->symbols, collec->firsts, collec->symbol_no, collec->terminal_no, collec->start, &end, 1);
  //
  //for (size_t i = collec->terminal_no; i < collec->symbol_no; ++i) {
  //  fprintf(stdout, "%s ", collec->symbols[i]->name);
  //  klr_print_terminal_set(stdout, collec, collec->firsts[i]);
  //  fputc('\n', stdout);
  //}
  //for (size_t i = collec->terminal_no; i < collec->symbol_no; ++i) {
  //  kev_bitset_delete(follows[i]);
  //}
  //free(follows);
  //
  //klr_print_collection(stdout, collec, true);
  //
  //S->id = 0;
  //L->id = 1;
  //R->id = 2;
  //id->id = 3;
  //star->id = 4;
  //assign->id = 5;
  //end->id = 6;
  //
  //rule1->id = 0;
  //rule2->id = 1;
  //rule3->id = 2;
  //rule4->id = 3;
  //rule5->id = 4;
  //rule6->id = 5;
  //
  //KlrConflictHandler* handler = klr_conflict_handler_create(NULL, conflict_report);
  //KlrTable* table = klr_table_create(collec, handler);
  //fputc('\n', stdout);
  //fputc('\n', stdout);
  //fputc('\n', stdout);
  //klr_print_symbols(stdout, collec);
  //fputc('\n', stdout);
  //fprintf(stdout, "%d ", 0);
  //klr_print_rule(stdout, rule1);
  //fputc('\n', stdout);
  //fprintf(stdout, "%d ", 1);
  //klr_print_rule(stdout, rule2);
  //fputc('\n', stdout);
  //fprintf(stdout, "%d ", 2);
  //klr_print_rule(stdout, rule3);
  //fputc('\n', stdout);
  //fprintf(stdout, "%d ", 3);
  //klr_print_rule(stdout, rule4);
  //fputc('\n', stdout);
  //fprintf(stdout, "%d ", 4);
  //klr_print_rule(stdout, rule5);
  //fputc('\n', stdout);
  //fputc('\n', stdout);
  //klr_print_goto_table(stdout, table);
  //fputc('\n', stdout);
  //klr_print_action_table(stdout, table);
  //klr_conflict_handler_delete(handler);
  //klr_collection_delete(collec);
  //klr_table_delete(table);
  //
  //klr_rule_delete(rule1);
  //klr_rule_delete(rule2);
  //klr_rule_delete(rule3);
  //klr_rule_delete(rule4);
  //klr_rule_delete(rule5);
  //klr_rule_delete(rule6);
  //klr_symbol_delete(L);
  //klr_symbol_delete(R);
  //klr_symbol_delete(S);
  //klr_symbol_delete(id);
  //klr_symbol_delete(star);
  //klr_symbol_delete(assign);
  //klr_symbol_delete(end);
  return 0;
}
