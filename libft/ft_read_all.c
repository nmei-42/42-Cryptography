/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_all.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/04 18:39:20 by nmei              #+#    #+#             */
/*   Updated: 2018/02/08 16:10:14 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <libft.h>

static t_rdlst	*rd_lstnew(char *str_frag, ssize_t ret)
{
	t_rdlst	*new;

	if ((new = (t_rdlst *)malloc(sizeof(*new))) == NULL)
		return (NULL);
	new->rd_ret = ret;
	new->str_frag = NULL;
	new->str_frag = (char *)ft_memdup(str_frag, ret);
	new->next = NULL;
	return (new);
}

static void		rd_lstadd(t_rdlst **rdlst, t_rdlst *new)
{
	if (rdlst && new)
	{
		if (*rdlst)
			(*rdlst)->next = new;
		*rdlst = new;
	}
}

static void		rd_lstfree(t_rdlst *start)
{
	t_rdlst	*curr;
	t_rdlst	*next;

	curr = start;
	while (curr)
	{
		if (curr->str_frag)
			free(curr->str_frag);
		next = curr->next;
		free(curr);
		curr = next;
	}
}

/*
**	stringify()
**	Given a linked list filled with buffers from read() and known total summed
**	len for all the buffers, return a 'stringified' version.
*/

static char		*stringify(t_rdlst *start, size_t len)
{
	char	*tmp;
	char	*tmp_start;
	char	*curr_str;
	t_rdlst	*curr;
	int		i;

	tmp = NULL;
	if ((tmp = (char *)malloc((len + 1) * sizeof(char))) == NULL)
		return (NULL);
	tmp[len] = '\0';
	tmp_start = tmp;
	curr = start;
	while (curr)
	{
		curr_str = curr->str_frag;
		i = -1;
		while (++i < (int)curr->rd_ret)
			*tmp++ = *curr_str++;
		curr = curr->next;
	}
	return (tmp_start);
}

/*
**	ft_read_all()
**	Read *EVERYTHING* from our fd and temporarily store our read buffers in a
**	linked list.
**
**	Ultimately returns the final length of the string (not counting the '\0'
**	added at the end) and stores the stringified version of the linked buffers
**	in 'result'.
**
**	If we encounter an error in our read, free whatever we have in our list and
**	return -1.
*/

ssize_t			ft_read_all(int fd, char **result)
{
	ssize_t	ret;
	size_t	len;
	char	buff[512];
	t_rdlst	*start;
	t_rdlst	*curr;

	start = NULL;
	ret = 0;
	len = 0;
	while ((ret = read(fd, buff, 512)) > 0)
	{
		if (ret == -1)
		{
			rd_lstfree(start);
			return (-1);
		}
		len += ret;
		rd_lstadd(&curr, rd_lstnew(buff, ret));
		if (start == NULL)
			start = curr;
	}
	*result = stringify(start, len);
	rd_lstfree(start);
	close(fd);
	return (len);
}
