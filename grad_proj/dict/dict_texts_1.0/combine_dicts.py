import os

def combine_copy(dicts_list, dest_name):
    comb_words = set()
    for dict_name in dicts_list:
        fp_dict = file(dict_name, 'rb')
        for word in fp_dict:
#            if word.strip() in comb_words:
#                print word
            comb_words.add(word.strip())
        fp_dict.close()
    fp_out = file(dest_name, 'wb')
    fp_tocws = file(os.path.join('../cws/dictionary', dest_name), 'wb')
    comb_list = list(comb_words)
    comb_list.sort()
    for word in comb_list:
        fp_out.write(word+'\n')
        fp_tocws.write(word+'\n')
    fp_out.close()
    fp_tocws.close()
    return

if __name__ == "__main__":
    dicts_list = ['crawl_cardi.txt', 'cardiovascular.txt']
    dest_name = 'final_dict.txt'
    combine_copy(dicts_list, dest_name)
