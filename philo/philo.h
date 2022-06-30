/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppajot <ppajot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 20:22:37 by ppajot            #+#    #+#             */
/*   Updated: 2022/06/30 20:30:19 by ppajot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>

typedef struct s_philo
{
	int				id;
	long			lasteat;
	long			*t_zero;
	int				mealcount;
	int				timetodie;
	int				timetoeat;
	int				timetosleep;
	int				*end;
	pthread_mutex_t	lasteatmutex;
	pthread_mutex_t	meals;
	pthread_mutex_t	*fork1;
	pthread_mutex_t	*fork2;
	pthread_mutex_t	*display;
	pthread_mutex_t	*start;
}		t_philo;


typedef struct s_data
{
	int				philnbr;
	int				timetodie;
	int				timetoeat;
	int				timetosleep;
	int				mealnbr;
	int				*mealcount;
	long			t_zero;
	int				end;
	pthread_t		*tid;
	pthread_mutex_t	*forkmutex;
	pthread_mutex_t	display;
	pthread_mutex_t	start;
	t_philo			*philo;
}		t_data;

#endif