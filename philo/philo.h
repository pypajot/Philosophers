/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppajot <ppajot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 20:22:37 by ppajot            #+#    #+#             */
/*   Updated: 2022/06/27 20:11:31 by ppajot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>



typedef struct s_data {
	int 			philnbr;
	int				timetodie;
	int				timetoeat;
	int				timetosleep;
	int				mealnbr;
	int				*mealcount;
	int				*forktab;
	int				philid;
	long			t_zero;
	pthread_t 		*tid;
	long			*lasteat;
	int				end;
	pthread_mutex_t *forkmutex;
	pthread_mutex_t endmutex;
	pthread_mutex_t display;
	pthread_mutex_t idmutex;
	pthread_mutex_t lasteatmutex;
	pthread_mutex_t meals;
}		t_data;

#endif