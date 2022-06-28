/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppajot <ppajot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 20:24:04 by ppajot            #+#    #+#             */
/*   Updated: 2022/06/27 20:11:28 by ppajot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

void	*philo(void *arg)
{
	t_data *data = (t_data *)arg;
	struct timeval tv;
	int		eat_sleep_think = 0;
	int		id;
	
	pthread_mutex_lock(&data->idmutex);	
	id = data->philnbr;
	pthread_mutex_unlock(&data->idmutex);
	pthread_mutex_lock(&data->endmutex);
	while (data->end)
	{
		pthread_mutex_unlock(&data->endmutex);
		if (eat_sleep_think == 2)
		{
			gettimeofday(&tv, 0);
			pthread_mutex_lock(&(data->display));
			printf("%li %i is thinking\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->t_zero,id);
			pthread_mutex_unlock(&data->display);
			gettimeofday(&tv, 0);
			eat_sleep_think = 0;
		}
		if (eat_sleep_think == 1)
		{
			//pthread_mutex_unlock(&(data->forkmutex[id + 1 % 2]));
			gettimeofday(&tv, 0);
			pthread_mutex_lock(&(data->display));
			printf("%li %i is sleeping\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->t_zero,id);
			pthread_mutex_unlock(&(data->display));
			usleep(data->timetosleep * 1000);
			eat_sleep_think = 2;
		}
		pthread_mutex_lock(&(data->forkmutex));
		if (data->forktab[0] && eat_sleep_think == 0)
		{			
			data->forktab[0] = 0;
			pthread_mutex_unlock(&(data->forkmutex));
			//pthread_mutex_lock(&(data->forkmutex[id + 1 % 2]));
			gettimeofday(&tv, 0);
			pthread_mutex_lock(&data->lasteatmutex);
			data->lasteat[id - 1] = tv.tv_sec * 1000 + tv.tv_usec / 1000;
			pthread_mutex_unlock(&data->lasteatmutex);
			pthread_mutex_lock(&(data->display));
			printf("%li %i is eating\n", data->lasteat[id - 1] - data->t_zero,id);
			pthread_mutex_unlock(&(data->display));
			usleep(data->timetoeat * 1000);
			pthread_mutex_lock(&(data->forkmutex));
			data->forktab[0] = 1;
			eat_sleep_think = 1;
		}
		pthread_mutex_unlock(&(data->forkmutex));	
		pthread_mutex_lock(&data->endmutex);
	}
	pthread_mutex_unlock(&data->endmutex);
	return (0);
}

int	check_end(void *arg)
{
	t_data *data = (t_data *)arg;
	struct timeval tv;
	int	i;
	
	usleep(100);
	while (1)
	{
		i = 0;
		while (i < 2)
		{
			gettimeofday(&tv, 0);
			pthread_mutex_lock(&data->lasteatmutex);
			if (tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->lasteat[i] > data->timetodie)
			{
				pthread_mutex_lock(&data->display);
				printf("%li %i died\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->t_zero, i + 1);
				pthread_mutex_lock(&data->endmutex);
				data->end = 0;
				pthread_mutex_unlock(&data->endmutex);
				pthread_mutex_unlock(&data->display);
				pthread_mutex_unlock(&data->lasteatmutex);
				return (0);
			}
			pthread_mutex_unlock(&data->lasteatmutex);
		}
	}
	return (0);
}

int	ft_atoi(char *str)
{
	int	res;

	res = 0;
	while (*str)
	{
		res = *str + res * 10;
		str++;
	}
	return (res);
}

int	check_arg(char *str)
{
	int	i;
	
	i = -1;
	while (str[++i])
		if (str[i] < '0' || str[i] > '9')
			return (0);
	return (1);
}

int	check_av(int ac, char **av)
{
	int	i;

	i = 0;
	if (ac < 5 || ac > 6)
		return (0);
	while (++i < ac)
	{
		if (!check_arg(av[i]))
			return (0);
		if (i == 1 && !ft_atoi(av[i]))
			return (0);
			
	}
	return (1);
}

t_data	init_data(int ac, char **av)
{
	t_data data;

	data.philnbr = ft_atoi(av[1]);
	data.timetodie = ft_atoi(av[2]);
	data.timetoeat = ft_atoi(av[3]);
	data.timetosleep = ft_atoi(av[4]);
	if (ac == 6)
		data.mealnbr = ft_atoi(av[5]);
	
}		

int	main(int ac, char **av)
{
	
	
	int	i = -1;
	t_data data;
	data.forktab[0] = 1;
	pthread_mutex_init(&data.display, 0);
	pthread_mutex_init(&data.lasteatmutex, 0);
	pthread_mutex_init(&data.idmutex, 0);
	pthread_mutex_init(&data.endmutex, 0);
	struct timeval tv;
	gettimeofday(&tv, 0);
	data.t_zero = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	data.end = 1;
	data.lasteat = (long *)malloc(2 * sizeof(long));
	while (++i < 2)
		data.lasteat[i] = data.t_zero;
	i = 0;
	while (i < 1)
	{
		pthread_mutex_init(&data.forkmutex, 0);
		i++;
	}
	i = 0;
	while (i < 2)
	{
		pthread_mutex_lock(&data.idmutex);
		data.philnbr = i + 1;		
		pthread_create(&data.tid[i], NULL, philo, &data);
		pthread_mutex_unlock(&data.idmutex);
		i++;
	}
	
	if (check_end(&data) == 0)
	{
		i = -1;
		while (++i < 2)
			pthread_join(data.tid[i], NULL);
		free(data.lasteat);
		return (0);	
	}
	return (0);
}