/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thelmy <thelmy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:16:49 by mrhelmy           #+#    #+#             */
/*   Updated: 2024/09/29 01:38:40 by thelmy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <termios.h>
# include <time.h>
# include <unistd.h>

# define BUFFER_SIZE 1
# define EXEC 1
# define BUILTIN 2
# define REDIR 3
# define PIPE 4
# define HEREDOC 5
# define MAXARGS 10000

typedef struct execcmd
{
	int				type;
	char			*argv[MAXARGS];
	char			*eargv[MAXARGS];
	char			echar[MAXARGS];
	int				cat_flag;
}					t_execcmd;

typedef struct s_norm
{
	void			**var1;
	void			**var2;
	t_execcmd		*cmd;
}					t_norm;

typedef struct s_context
{
	int				last_exit_status;
}					t_context;

typedef struct env
{
	char			*variable;
	char			*value;
	char			**ev;
	struct env		*next;
}					t_env;

typedef struct export
{
	char			*variable;
	char			*value;
	struct export	*next;
}	t_export;

t_env				*create_env_nodes(char *variable_content,
						char *value_content);
t_env				*storing_env(char **ev);
t_export			*storing_export(char **ev);
t_export			*create_export_nodes(char *variable_content,
						char *value_content);

void				env_func(t_env *env, int *last_exit_status);
void				pwd(int *last_exit_status);
void				cd(char **args, t_env **envir, int *last_exit_status);
void				echo(char **command, char echar[MAXARGS]);

void				sort_array(t_export **export_arr, int count);
void				printing_export(t_export **export_arr, int export_count);
void				export_no_arg(t_export *export, int *last_exit_status);

int					is_valid_identifier(const char *str);
void				update_env(t_env **env, char *variable, char *value,
						int *last_exit_status);

void				export_with_args(t_norm x, t_export **export, int ac,
						int *last_exit_status);
int					num_strncmp(char *s1, char *s2);

void				unset_env(t_env **env, char *variable);
void				unset_export(t_export **exp, int ac, char **av,
						int *last_exit_status);
void				unset(t_env **env, int ac, char **av,
						int *last_exit_status);

void				update_env_for_cd(t_env **env, char *variable, char *value);

bool				is_only_n(const char *str);

char				*substr_before_char(char *str, char c);
char				*substr_after_char(char *str, char c);
bool				ft_strcmp(char *s1, char *s2);
int					listsize(t_export *export);
int					num_strncmp(char *s1, char *s2);

size_t				ft_strlen(const char *s);
char				*ft_strdup(const char *s1);
size_t				ft_strlcpy(char *dst, const char *src, size_t dstsize);
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_itoa(int n);
int					wordcount(char *str, char sep);
char				**getwords(char **arr, char *s, char c, int word);
char				**ft_split(char const *s, char c);
char				*getcopyenv(char *str, t_env **envir);
int					ft_isalnum(int c);
int					ft_num_len(int n);

int					env_path(t_env *env, int *last_exit_status);

typedef struct cmd
{
	int				type;
}					t_cmd;

typedef struct heredoc
{
	int				type;
	char			*argv;
	char			*eargv;
	void			*next;
}					t_heredoc;

typedef struct redircmd
{
	int				type;
	struct cmd		*cmd;
	char			*file;
	char			*efile;
	int				mode;
	int				fd;
}					t_redircmd;

typedef struct pipecmd
{
	int				type;
	struct cmd		*left;
	struct cmd		*right;
}					t_pipecmd;

typedef struct main
{
	struct cmd		*cmd;
	struct cmd		*main_cmd;
	struct heredoc	*heredoc;
	char			*command;

	struct pipecmd	*pcmd;
	struct execcmd	*ecmd;
	struct redircmd	*rcmd;
	int				saved_stdin;
	int				saved_stdout;
	int				status;
	int				p[2];
	int				*start;
	char			*input;
	int				*cat_counter;
	int				*stop_cat;
	int				*has_heredoc;
	int				*executed_heredoc;
	int				*stop_cat_right_child;
}					t_main;

typedef struct parseexec
{
	char			*q;
	char			*eq;
	int				tok;
	int				argc;
}					t_parseexec;

void				initialize_shell(char **ev, t_env **envir, t_export **exp,
						t_context *context);
t_main				initialize_main(t_main x);
void				runcmd(t_main main, t_env **envir, t_export **exp,
						int *last_exit_status);

int					fork1(void);
void				panic(char *s);

struct cmd			*parsepipe(char **ps, char *es, struct heredoc **heredoc,
						int *last_exit_status);
t_main				parsecmd(char *s, t_env *envir, int *last_exit_status);
struct cmd			*parseexec(char **ps, char *es, struct heredoc **heredoc,
						int *last_exit_status);

t_cmd				*parseredirs(t_norm x, t_cmd *cmd, char *es,
						int *last_exit_status);

int					gettoken(char **ps, char *es, char **q, char **eq);
struct cmd			*pipecmd(struct cmd *left, struct cmd *right);

struct cmd			*nulterminate(struct cmd *cmd, t_env *envir,
						int *last_exit_status);
struct cmd			*execcmd(void);
struct cmd			*redircmd(struct cmd *subcmd, t_norm x, int mode, int fd);

void				redircmd_h(char *argv, char *eargv,
						struct heredoc **heredoc);
struct cmd			*expand_tree(struct cmd *cmd, t_env *envir,
						int *last_exit_status);

t_env				*execute_builtin(t_norm x, char echar[MAXARGS],
						int *last_exit_status, t_export **exp);
void				modify_args(char **args, t_env *envir,
						int *last_exit_status);
bool				is_builtin(char *command);
char				*allocate_result(char *arg, t_env *envir,
						int *last_exit_status);
int					handle_exit_status(char *res, int *last_exit_status);
int					handle_var_expansion(char *res, char *arg, int *i,
						t_env *envir);
int					handle_exit_status_len(int *last_exit_status);
int					handle_var_len(char *arg, int *i, t_env *envir);

void				setup_signals(void);
void				sigint_handler(int sig, siginfo_t *info, void *context);
void				configure_terminal_behavior(void);
void				update_signals(void);

char				*ft_strcat(char *dest, const char *src);
char				*ft_strjoin(char const *s1, char const *s2);
char				*ft_strnstr(const char *big, const char *little,
						size_t len);
struct cmd			*print_tree(struct cmd *cmd);

struct cmd			*remove_quotes(struct cmd *cmd);

void				update_export(t_export **export, char *variable,
						char *value, int *last_exit_status);
char				*get_next_line(int fd);
char				*gnl_strchr(const char *s, int c);
void				*gnl_memmove(void *dst, const void *src, size_t len);
size_t				enhanced_strlen(char *str, int c);
int					gnl_free_str(char **str);
char				*gnl_strjoin(char *s1, char *s2);
void				builtin_exit(char **args, int *last_exit_status);

void				rl_replace_line(const char *text, int clear_undo);

void				free_env(t_env *env);
void				free_exp_node(t_export *node);
char				**free_arr(char **arr);
void				free_env_node(t_env *node);
void				free_export(t_export *export);
void				freecmd(struct cmd *cmd, int x);
void				freeheredoc(struct heredoc *heredoc);
void				free_double_pointer(char **str);
char				*ft_strcpy(char *dst, const char *src);
char				*ft_strchr(const char *s, int c);
void				*ft_memset(void *b, int c, size_t len);
int					ft_isdigit(int c);
int					ft_isalpha(int c);
void				fill_env(t_env **env, char **ev);
char				**convert_env(t_env **env);

char				*find_path(char *cmd, char **envp);
char				*heredoc_exec(t_main main, int *last_exit_status,
						int *has_heredoc);
void				redir(t_main main, t_env **envir, t_export **exp,
						int *last_exit_status);
void				exec(t_main main, t_env **envir, t_export **exp,
						int *last_exit_status);
void				exec_pipe(t_main main, t_env **envir, t_export **exp,
						int *last_exit_status);

void				free_wrong_parsing(char *s, struct cmd *cmd, t_main main,
						int *last_exit_status);
int					check_quotes(char *str);
int					peek(char **ps, char *es, char *toks);
void				cat_with_heredoc(t_main main, t_env **envir,
						t_export **exp);
void				free_failed_exec(t_main main, t_env **envir, t_export **exp,
						int *last_exit_status);
void				run_free_left_child(t_main main, t_env **envir,
						t_export **exp, int *last_exit_status);

int					print_token_error(t_cmd *cmd, int *last_exit_status);
t_execcmd			*initialize_execcmd(t_cmd **ret, int *last_exit_status);
int					get_argv(t_norm *x, t_cmd **ret, char *es,
						int *last_exit_status);
#endif