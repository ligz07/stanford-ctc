#pragma once
#include "lm.h"
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
//#include <boost/progress.hpp>

// TODO FIXME PARAM Import these parameters in using boost.python
namespace PREFIX_TREE_CONSTS
{
    const std::string SPACE = "<space>";
    const std::string SPECIALS_LIST[4] = {"<space>", "[NOISE]", "[VOCALIZED-NOISE]", "[LAUGHTER]"};
};

struct PrefixTreeNode
{
    PrefixTreeNode() : is_prefix(false), is_word(false) {}
    bool is_prefix;
    bool is_word;
    int word_id;
    typedef boost::shared_ptr<PrefixTreeNode> Ptr;
    boost::unordered_map<int, Ptr> children;
    boost::unordered_map<int, float> probScore;
};

class PrefixTree
{
  public:
    PrefixTree(const std::string& char_map_file,
               const std::string& word_list_file,
               const std::string& arpa_lm_file) :
        lm(arpa_lm_file), path_count(0), root(new PrefixTreeNode)
    {
        lm.load_chars(char_map_file);
        lm.load_words(word_list_file);

        root->is_prefix = true;
        space = lm.char_map[PREFIX_TREE_CONSTS::SPACE];

        BOOST_FOREACH (std::string w, PREFIX_TREE_CONSTS::SPECIALS_LIST)
        {
            PrefixTreeNode::Ptr n(new PrefixTreeNode());
            n->is_word = true;
            n->word_id = lm.get_word_id(w);
            root->children.insert(std::make_pair(lm.char_map[w], n));
        }

        //boost::progress_display show_progress(lm.word_list.size());
        BOOST_FOREACH(std::string word, lm.word_list)
        {
            add_path(word, root, lm.get_word_id(word));
            //++show_progress;
        }

		std::cout<<"look a head begin"<<std::endl;
        lookahead(root);
		std::cout<<"look a head end"<<std::endl;
		//check(root);
    }
	void check(PrefixTreeNode::Ptr rootNode)
	{
		int k = 5858;
		std::list<PrefixTreeNode::Ptr> nodelist;
		nodelist.push_back(rootNode);
		//std::cout<<rootNode->probScore[k]<<std::endl;
		while (nodelist.size() > 0)
		{
			PrefixTreeNode::Ptr node = nodelist.front();
			nodelist.pop_front();
			std::cout<<node->probScore[k]<<std::endl;
			for( boost::unordered::unordered_map<int,PrefixTreeNode::Ptr>::iterator iter1 = node->children.begin(); iter1 != node->children.end(); iter1++)
			{   
				PrefixTreeNode::Ptr iter = iter1->second;
				nodelist.push_back(iter);
				std::cout<<iter->probScore[k]<<" ";
			}
			std::cout<<std::endl;
		}
	}
    void lookahead(PrefixTreeNode::Ptr rootNode)
    {
        for( boost::unordered::unordered_map<int,PrefixTreeNode::Ptr>::iterator iter1 = rootNode->children.begin(); iter1 != rootNode->children.end(); iter1++)
        {
            PrefixTreeNode::Ptr iter = iter1->second;
	//    std::cout<<"parent:"<<rootNode->word_id<<" child:"<<iter1->first<<std::endl;
            if (!iter->is_prefix && iter->is_word)
            {
                for (int j = 0; j < lm.word_ids.size(); j++)
                {
                    int wid = lm.word_ids[j];
                    float score = lm.bg_prob(wid, iter->word_id);
					if (score > -0.00000001 && score < 0.00000001)
					{
						std::cout<<score<<" "<<wid<<" "<<iter->word_id<<std::endl;
					}
                    if (rootNode->probScore.find(wid) != rootNode->probScore.end())
                    {
                       if (score > rootNode->probScore[wid])
                       {
                           rootNode->probScore[wid] = score;
                       }
                    }
                    else
                    {
                        rootNode->probScore[wid] = score;
                    }
                }
            }
            else if (iter->is_prefix)
            {
                lookahead(iter);
                for (int j = 0; j < lm.word_ids.size(); j++)
                {
                    int wid = lm.word_ids[j];
                    if (iter->probScore.find(wid) == iter->probScore.end())
                    {
                        continue;
                    }

                    float score = iter->probScore[wid];
                    if (rootNode->probScore.find(wid) != rootNode->probScore.end())
                    {
                       if (score > rootNode->probScore[wid])
                       {
                           rootNode->probScore[wid] = score;
                       }
                    }
                    else
                    {
                        rootNode->probScore[wid] = score;
                    }
                }
            }
        }
    }
    void add_path(const std::string& prefix, PrefixTreeNode::Ptr node, int word_id);
    int path_count;

  //protected:
    LM lm;
    PrefixTreeNode::Ptr root;
    int space;
};
