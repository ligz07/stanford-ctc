#include "prefix_tree.h"

void PrefixTree::add_path(const std::string& prefix, PrefixTreeNode::Ptr node, int word_id)
{
    setlocale(LC_CTYPE, "zh_CN.utf8");
    path_count++;

    int nLen = prefix.length();
    wchar_t word[10];
    memset(word, 0, sizeof(wchar_t)*10);
    int k = mbstowcs(word, prefix.c_str(),nLen);
    //std::cout<<k<<std::endl;
    std::wstring wstr(word);
    std::wstring wp = wstr.substr(0,1);;
    std::wstring wrest = wstr.substr(1);
    //std::string p = prefix.substr(0, 1);
    char buff[30];
    k = wcstombs(buff, wp.c_str(), 30);
    //std::cout<<k<<" "<<buff<<std::endl;
    std::string p(buff);
    //std::string rest = prefix.substr(1);
    k = wcstombs(buff, wrest.c_str(), 30);
    std::string rest(buff);
    PrefixTreeNode::Ptr next(new PrefixTreeNode);
    next->word_id = lm.unk;
    if (lm.char_map.find(p) == lm.char_map.end())
    {
        return;
    }
    if (node->children.find(lm.char_map[p]) == node->children.end())
    {
        node->children[lm.char_map[p]] = next;
        //std::cout<<"add"<<lm.char_map[p]<<std::endl;
    }
    else
        next = node->children[lm.char_map[p]];

    if (rest.length() == 0)
    {
        next->is_word = true;
        next->word_id = word_id;
    //    std::cout<<lm.char_map[p]<<"----"<<p<<std::endl;
        return;
    }
    else
    {
        next->is_prefix = true;
        add_path(rest, next, word_id);
    }
}
