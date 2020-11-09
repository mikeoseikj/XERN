#ifndef EXT2_H
#define EXT2_H


struct ext2_super_block 
{
	unsigned int    s_inodes_count;		
	unsigned int	s_blocks_count;		
	unsigned int	s_r_blocks_count;	
	unsigned int	s_free_blocks_count;	
	unsigned int	s_free_inodes_count;	
	unsigned int	s_first_data_block;	
	unsigned int	s_log_block_size;	
	unsigned int	s_log_frag_size;	
	unsigned int	s_blocks_per_group;
	unsigned int	s_frags_per_group;	
	unsigned int	s_inodes_per_group;
	unsigned int	s_mtime;		
	unsigned int	s_wtime;		
	unsigned short	s_mnt_count;		
	unsigned short	s_max_mnt_count;	
	unsigned short	s_magic;		
	unsigned short	s_state;
	unsigned short	s_errors;		
	unsigned short	s_minor_rev_level; 	
	unsigned int	s_lastcheck;		
	unsigned int	s_checkinterval;	
	unsigned int	s_creator_os;		
	unsigned int	s_rev_level;		
	unsigned short	s_def_resuid;		
	unsigned short	s_def_resgid;

    
    /* These fields are for EXT2_DYNAMIC_REV superblocks only */
	unsigned int	s_first_ino; 		
	unsigned short  s_inode_size; 		
	unsigned short	s_block_group_nr; 	
	unsigned int	s_feature_compat; 	
	unsigned int	s_feature_incompat; 	
	unsigned int	s_feature_ro_compat; 	
	unsigned char	s_uuid[16];	
	char	        s_volume_name[16]; 
	char	        s_last_mounted[64]; 	
    unsigned int    s_algorithm_usage_bitmap;

    unsigned char   s_prealloc_blocks;
    unsigned char   s_prealloc_dir_blocks;
    unsigned short  s_padding1;
	unsigned int	s_reserved[204];
};


struct ext2_group_desc
{
	unsigned int	bg_block_bitmap;	
	unsigned int	bg_inode_bitmap;	
	unsigned int	bg_inode_table;		
	unsigned short	bg_free_blocks_count;	
	unsigned short	bg_free_inodes_count;
	unsigned short	bg_used_dirs_count;	
	unsigned short	bg_pad;
	unsigned int    bg_reserved[3];
};

struct ext2_inode 
{
	unsigned short	i_mode;		
	unsigned short	i_uid;		
	unsigned int	i_size;		
	unsigned int	i_atime;	
	unsigned int	i_ctime;	
	unsigned int	i_mtime;	
	unsigned int	i_dtime;	
	unsigned short	i_gid;		
	unsigned short	i_links_count;
	unsigned int	i_blocks;	
	unsigned int	i_flags;	
	unsigned int    i_osd1;     /* OS dependent 1*/
	unsigned int	i_block[15];   
    unsigned int    i_pad[5];         
    unsigned int	i_reserved[2];                 
};

struct ext2_dir_entry_2 
{
	unsigned int	inode;			
	unsigned short	rec_len;		
	unsigned char	name_len;		
	unsigned char	file_type;
	char	        name[255];      	// Not null terminated
};


#endif